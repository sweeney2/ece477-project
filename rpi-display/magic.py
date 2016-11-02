#!/usr/bin/python3

import datetime
import sys
import sdl2
import sdl2.ext

RESOURCES = sdl2.ext.Resources(__file__, 'resources')
SCREEN_WIDTH = 720  # 270
SCREEN_HEIGHT = 1280  # 480
PADDING = 40
HR_WIDTH = 1

FONT_COMP = {
        120: [41, 33],
        36: [12, 10],
        }


class SoftwareRenderer(sdl2.ext.SoftwareSpriteRenderSystem):

    def __init__(self, window):
        super(SoftwareRenderer, self).__init__(window)
        self.componenttypes = (sdl2.ext.Sprite,)#, DynamicSprite)

    def render(self, components):
        sdl2.ext.fill(self.surface, sdl2.ext.Color(0, 0, 0))
        components = [c.sprite if type(c) == DynamicSprite else c for c in components]
        super(SoftwareRenderer, self).render(components)


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

    world.add_system(tu)
    world.add_system(du)


    factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
    fm = sdl2.ext.FontManager(RESOURCES.get_path('Roboto-Regular.ttf'), size=32)
    usb_s = factory.from_color(sdl2.ext.Color(255,255,255), size=(120, 80))
    water_s = factory.from_color(sdl2.ext.Color(255,255,255), size=(120, 80))
    quote_s = factory.from_text('"This is a very long quote, and it should wrap when it gets too big for the screen but it probably won\'t."', fontmanager=fm, width=SCREEN_WIDTH-PADDING*2)
    hr_s = factory.from_color(sdl2.ext.Color(255,255,255), size=(SCREEN_WIDTH-PADDING*2, HR_WIDTH))


    usb = StaticEntity(world, usb_s, PADDING, PADDING)
    water = StaticEntity(world, water_s, PADDING * 2 + usb_s.size[0], PADDING)

    hr1 = StaticEntity(world, hr_s, PADDING, usb_s.y + usb_s.size[1] + PADDING)

    clock = ClockEntity(world, 120, PADDING, hr_s.y + hr_s.size[1] + PADDING - FONT_COMP[120][0])
    calendar = CalendarEntity(world, 36, PADDING, 300)

    quote = StaticEntity(
            world,
            quote_s,
            (SCREEN_WIDTH - quote_s.size[0]) // 2,
            SCREEN_HEIGHT - quote_s.size[1] - PADDING
            )

    running = True
    while running:
        events = sdl2.ext.get_events()
        for event in events:
            if event.type == sdl2.SDL_QUIT:
                running = False
                break
        world.process()
    sdl2.ext.quit()

if __name__ == '__main__':
    sys.exit(run())

