#!/usr/bin/python3

import datetime
import sys
import sdl2
import sdl2.ext

RESOURCES = sdl2.ext.Resources(__file__, 'resources')
SCREEN_WIDTH = 720  # 270
SCREEN_HEIGHT = 1280  # 480
HPADDING = 40
VPADDING = 40
HR_WIDTH = 1

FONT_COMP = {
        120: [41, 33],
        36: [12, 10],
        }

camera = [0.0, 0.0,]  # globals are fun and easy!


class SoftwareRenderer(sdl2.ext.SoftwareSpriteRenderSystem):

    def __init__(self, window):
        super(SoftwareRenderer, self).__init__(window)
        self.componenttypes = (sdl2.ext.Sprite,)#, DynamicSprite)

    def render(self, components):
        sdl2.ext.fill(self.surface, sdl2.ext.Color(0, 0, 0))
        super(SoftwareRenderer, self).render(self.expand_sprites(components), int(camera[0]), int(camera[1]))

    def expand_sprites(self, components):
        expanded = []
        for component in components:
            if type(component) is DynamicSprite:
                expanded.append(component.sprite)
            elif type(component) is SpriteGroup:
                expanded.extend(component.sprites)
            else:
                expanded.append(component)
        return expanded


class TimeUpdater(sdl2.ext.Applicator):

    def __init__(self):
        super(TimeUpdater, self).__init__()
        self.componenttypes = (DynamicSprite, Clock,)
        self.fontmanager = sdl2.ext.FontManager(
                RESOURCES.get_path('Roboto-Regular.ttf'),
#               bg_color=sdl2.ext.Color(255,0,0)
                )
        self.factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)

    def process(self, world, componentsets):
        for ds, clock in componentsets:
            if clock.update() or ds.is_new:
                position = ds.sprite.position
                ds.is_new = False
                ds.sprite = self.factory.from_text(
                        clock.get_time(),
                        size = ds.text_size,
                        fontmanager=self.fontmanager
                        )
                ds.sprite.position = position


class ScreenTweener(sdl2.ext.System):

    def __init__(self):
        super(ScreenTweener, self).__init__()
        self.componenttypes = (CameraManager,)

    def process(self, world, components):
        if not components:
            return
        for c in components:
            if not c.dead:
                c.update_camera()
                return


class DateUpdater(sdl2.ext.Applicator):

    def __init__(self):
        super(DateUpdater, self).__init__()
        self.componenttypes = (DynamicSprite, Calendar,)
        self.fontmanager = sdl2.ext.FontManager(
                RESOURCES.get_path('Roboto-Regular.ttf'),
#               bg_color=sdl2.ext.Color(255,0,0)
                )
        self.factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)

    def process(self, world, componentsets):
        for ds, calendar in componentsets:
            if calendar.update() or ds.is_new:
                position = ds.sprite.position
                ds.is_new = False
                ds.sprite = self.factory.from_text(
                        calendar.get_date(),
                        size = ds.text_size,
                        fontmanager=self.fontmanager
                        )
                ds.sprite.position = position


# Entities

class StaticEntity(sdl2.ext.Entity):

    def __init__(self, world, sprite, posx=0, posy=0, screenx=0, screeny=0):
        self.sprite = sprite
        self.sprite.position = SCREEN_WIDTH*screenx+posx, SCREEN_HEIGHT*screeny+posy


class ClockEntity(sdl2.ext.Entity):

    def __init__(self, world, size=16, posx=0, posy=0, screenx=0, screeny=0):
        self.clock = Clock()
        self.dynamicsprite = DynamicSprite(size)
        self.dynamicsprite.sprite.position = SCREEN_WIDTH*screenx+posx, SCREEN_HEIGHT*screeny+posy


class CalendarEntity(sdl2.ext.Entity):

    def __init__(self, world, size=16, posx=0, posy=0, screenx=0, screeny=0):
        self.calendar = Calendar()
        self.dynamicsprite = DynamicSprite(size)
        self.dynamicsprite.sprite.position = SCREEN_WIDTH*screenx+posx, SCREEN_HEIGHT*screeny+posy


class ArrowEntity(sdl2.ext.Entity):

    def __init__(self, world, n_text, s_text, e_text, w_text, posx=0, posy=0, screenx=0, screeny=0):
        factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
        fm = sdl2.ext.FontManager(RESOURCES.get_path('Roboto-Regular.ttf'), size=24)
        n = factory.from_text(n_text, fontmanager=fm)
        s = factory.from_text(s_text, fontmanager=fm)
        e = factory.from_text(e_text, fontmanager=fm)
        w = factory.from_text(w_text, fontmanager=fm)
        filename = 'dir_{}{}{}{}.bmp'.format(
                'n' if n_text.strip() else '',
                's' if s_text.strip() else '',
                'e' if e_text.strip() else '',
                'w' if w_text.strip() else ''
                )
        arrows = factory.from_image(RESOURCES.get_path(filename))

        self.spritegroup = SpriteGroup(
                (arrows, (SCREEN_WIDTH*screenx+0, 0,),),
                (n, (SCREEN_WIDTH*screenx+ (arrows.size[0] // 2 - n.size[0] // 2), SCREEN_HEIGHT*screeny+(0 - n.size[1] - 0),),),
                (s, (SCREEN_WIDTH*screenx+ (arrows.size[0] // 2 - s.size[0] // 2), SCREEN_HEIGHT*screeny+(0 + arrows.size[1] + 0),),),
                (e, (SCREEN_WIDTH*screenx+ (arrows.size[0] + 8), SCREEN_HEIGHT*screeny+(arrows.size[1] // 2 - e.size[1] // 2 - 0),),),
                (w, (SCREEN_WIDTH*screenx+ (0 - w.size[0] - 8), SCREEN_HEIGHT*screeny+(arrows.size[1] // 2 - w.size[1] // 2 - 0),),),
                )
        self.spritegroup.set_pos(posx, posy)


class ScreenCameraEntity(sdl2.ext.Entity):

    def __init__(self, world, direction):
        self.cameramanager = CameraManager(direction)


# Helper classes

class DynamicSprite(sdl2.ext.Sprite):

    def __init__(self, text_size=16):
        factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
        self.sprite = factory.from_color(sdl2.ext.Color(255,255,255), size=(1,1))
        self.is_new = True
        self.text_size = text_size

    @property
    def depth(self):
        return self.sprite.depth


class SpriteGroup(sdl2.ext.Sprite):

    def __init__(self, *sprite_info):
        self.sprites = [s[0] for s in sprite_info]
        self.positions = [p[1] for p in sprite_info]

    @property
    def depth(self):
        return self.sprites[0].depth if self.sprites else 0

    def set_pos(self, posx, posy):
        for i in range(len(self.sprites)):
            self.sprites[i].position = (
                    posx + self.positions[i][0],
                    posy + self.positions[i][1],
                    )


class CameraManager(object):

    def __init__(self, direction):
        super(CameraManager, self).__init__()
        self.direction = direction
        self.target = None
        if direction == 'up':
            self.target = camera[1] + SCREEN_HEIGHT
        if direction == 'down':
            self.target = camera[1] - SCREEN_HEIGHT
        if direction == 'left':
            self.target = camera[0] + SCREEN_WIDTH
        if direction == 'right':
            self.target = camera[0] - SCREEN_WIDTH
        self.scale = 0.00002
        self.base = 1
        self.thresh = 1
        self.dead = False

    def update_camera(self):
        diff_x = abs(camera[0]) % SCREEN_WIDTH
        diff_y = abs(camera[1]) % SCREEN_HEIGHT
        rate_x = diff_x * (SCREEN_WIDTH - diff_x) * self.scale + self.base
        rate_y = diff_y * (SCREEN_HEIGHT - diff_y) * self.scale + self.base

        if self.direction == 'up':
            camera[1] = self.target  # += rate_y
            if abs(self.target - camera[1]) < self.thresh:
                camera[1] = self.target
                self.dead = True
        if self.direction == 'down':
            camera[1] = self.target  # -= rate_y
            if abs(self.target - camera[1]) < self.thresh:
                camera[1] = self.target
                self.dead = True
        if self.direction == 'left':
            camera[0] = self.target  # += rate_x
            if abs(self.target - camera[0]) < self.thresh:
                camera[0] = self.target
                self.dead = True
        if self.direction == 'right':
            camera[0] = self.target  # -= rate_x
            if abs(self.target - camera[0]) < self.thresh:
                camera[0] = self.target
                self.dead = True


class Clock(object):

    def __init__(self):
        super(Clock, self).__init__()
        self.lastnow = datetime.datetime.now()

    def update(self):
        now = datetime.datetime.now()
        if now.minute - self.lastnow.minute:
            self.lastnow = now
            return True
        return False

    def get_time(self):
        return self.lastnow.strftime('%I:%M %p').strip('0')


class Calendar(object):

    def __init__(self):
        super(Calendar, self).__init__()
        self.lastnow = datetime.datetime.now()

    def update(self):
        now = datetime.datetime.now()
        if now.day - self.lastnow.day:
            self.lastnow = now
            return True
        return False

    def get_date(self):
        return self.lastnow.strftime('%A, %B %d').upper()



# Main loop

def run():
    sdl2.ext.init()
    window = sdl2.ext.Window('Magic Mirror', size=(SCREEN_WIDTH, SCREEN_HEIGHT))
    window.show()

    world = sdl2.ext.World()

    renderer = SoftwareRenderer(window)
    world.add_system(renderer)

    tu = TimeUpdater()
    du = DateUpdater()
    st = ScreenTweener()

    world.add_system(tu)
    world.add_system(du)
    world.add_system(st)


    factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
    fm = sdl2.ext.FontManager(RESOURCES.get_path('Roboto-Regular.ttf'), size=32)
    usb_s = factory.from_image(RESOURCES.get_path('usb.bmp'))
    water_s = factory.from_image(RESOURCES.get_path('water.bmp'))
    light_s = factory.from_image(RESOURCES.get_path('light.bmp'))
    images_s = factory.from_image(RESOURCES.get_path('images.bmp'))
    usb_s_ = factory.from_image(RESOURCES.get_path('usb.bmp'))
    water_s_ = factory.from_image(RESOURCES.get_path('water.bmp'))
    light_s_ = factory.from_image(RESOURCES.get_path('light.bmp'))
    images_s_ = factory.from_image(RESOURCES.get_path('images.bmp'))
    hr_s = factory.from_color(sdl2.ext.Color(255,255,255), size=(SCREEN_WIDTH-HPADDING*2, HR_WIDTH))


    usb = StaticEntity(world, usb_s, HPADDING, VPADDING)
    water = StaticEntity(world, water_s, HPADDING + usb_s.position[0] + usb_s.size[0], VPADDING)
    light = StaticEntity(world, light_s, HPADDING + water_s.position[0] + water_s.size[0], VPADDING)
    images = StaticEntity(world, images_s, HPADDING + light_s.position[0] + light_s.size[0], VPADDING)
    usb = StaticEntity(world, usb_s_, HPADDING, VPADDING, 1, 0)
    water = StaticEntity(world, water_s_, HPADDING + usb_s.position[0] + usb_s.size[0], VPADDING, 1, 0)
    light = StaticEntity(world, light_s_, HPADDING + water_s.position[0] + water_s.size[0], VPADDING, 1, 0)
    images = StaticEntity(world, images_s_, HPADDING + light_s.position[0] + light_s.size[0], VPADDING, 1, 0)

    hr1 = StaticEntity(world, hr_s, HPADDING, usb_s.y + usb_s.size[1] + VPADDING)

    clock = ClockEntity(world, 120, HPADDING, hr_s.y + hr_s.size[1] + VPADDING * 2 - FONT_COMP[120][0])
    calendar = CalendarEntity(world, 36, HPADDING, clock.dynamicsprite.sprite.y + FONT_COMP[120][0] + VPADDING + 85 - FONT_COMP[36][1])
    home_arrow = ArrowEntity(world, 'Photo', 'Lights', 'Water', 'Timer', (SCREEN_WIDTH - 120) // 2, SCREEN_HEIGHT - 200)
    graph_arrow_day = ArrowEntity(world, 'Weekly', ' ', ' ', 'Home', (SCREEN_WIDTH - 120) // 2, SCREEN_HEIGHT - 200, 1, 0)

    camera_entity = None


    running = True
    while running:
        if camera_entity and camera_entity.cameramanager.dead:
            world.delete(camera_entity)
            camera_entity = None
        events = sdl2.ext.get_events()
        for event in events:
            if event.type == sdl2.SDL_QUIT:
                running = False
                break
            if event.type == sdl2.SDL_KEYUP:
                if event.key.keysym.sym == sdl2.SDLK_q:
                    running = False
                    break
                if event.key.keysym.sym == sdl2.SDLK_UP:
                    if not camera_entity:
                        camera_entity = ScreenCameraEntity(world, 'up')
                if event.key.keysym.sym == sdl2.SDLK_DOWN:
                    if not camera_entity:
                        camera_entity = ScreenCameraEntity(world, 'down')
                if event.key.keysym.sym == sdl2.SDLK_LEFT:
                    if not camera_entity:
                        camera_entity = ScreenCameraEntity(world, 'left')
                if event.key.keysym.sym == sdl2.SDLK_RIGHT:
                    if not camera_entity:
                        camera_entity = ScreenCameraEntity(world, 'right')
        world.process()
    sdl2.ext.quit()

if __name__ == '__main__':
    sys.exit(run())

