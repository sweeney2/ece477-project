#!/usr/bin/python3

import datetime
import sys
import sdl2
import sdl2.ext

RESOURCES = sdl2.ext.Resources(__file__, 'resources')
SCREEN_WIDTH = 720  # 270
SCREEN_HEIGHT = 1280  # 480


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
        self.fontmanager = sdl2.ext.FontManager('resources/Roboto-Regular.ttf', size=100)

    def process(self, world, componentsets):
        for ds, clock in componentsets:
            if clock.update():
                factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
                print('Replacing sprite')
                position = ds.sprite.position
                ds.sprite = factory.from_text(
                        clock.get_time(),
                        fontmanager=self.fontmanager
                        )
                ds.sprite.position = position


# Entities

class StaticEntity(sdl2.ext.Entity):

    def __init__(self, world, sprite, posx=0, posy=0, screenx=0, screeny=0):
        self.sprite = sprite
        self.sprite.position = SCREEN_WIDTH*screenx+posx, SCREEN_HEIGHT*screeny+posy


class DynamicEntity(sdl2.ext.Entity):

    def __init__(self, world, posx=0, posy=0, screenx=0, screeny=0):
        self.dynamicsprite = DynamicSprite()


class ClockEntity(sdl2.ext.Entity):

    def __init__(self, world, posx=0, posy=0, screenx=0, screeny=0):
        self.clock = Clock()
        self.dynamicsprite = DynamicSprite()
        fontmanager = sdl2.ext.FontManager('resources/Roboto-Regular.ttf', size=100)
        factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
        self.dynamicsprite.sprite = factory.from_text(
            self.clock.get_time(),
            fontmanager=fontmanager
            )
        self.dynamicsprite.sprite.position = SCREEN_WIDTH*screenx+posx, SCREEN_HEIGHT*screeny+posy


# Helper classes

class DynamicSprite(sdl2.ext.Sprite):

    def __init__(self):
        factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
        self.sprite = factory.from_color(sdl2.ext.Color(255,255,255), size=(10,10))

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
        return self.lastnow.strftime('%I:%M %p')



# Main loop

def run():
    sdl2.ext.init()
    window = sdl2.ext.Window('Magic Mirror', size=(SCREEN_WIDTH, SCREEN_HEIGHT))
    window.show()

    world = sdl2.ext.World()

    renderer = SoftwareRenderer(window)
    world.add_system(renderer)

    tu = TimeUpdater()
    world.add_system(tu)


    factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
    fm = sdl2.ext.FontManager(RESOURCES.get_path('Roboto-Regular.ttf'), size=32)

    usb_sprite = factory.from_color(sdl2.ext.Color(255,255,255), size=(120, 80))
    usb = StaticEntity(world, usb_sprite, 20, 20)

    water_sprite = factory.from_color(sdl2.ext.Color(255,255,255), size=(120, 80))
    water = StaticEntity(world, water_sprite, SCREEN_WIDTH-water_sprite.size[0]-20, 20)

    quote_sprite = factory.from_text('"This is a very long quote, and it should wrap when it gets too big for the screen but it probably won\'t."', fontmanager=fm, width=SCREEN_WIDTH-40)
    quote = StaticEntity(world, quote_sprite, (SCREEN_WIDTH-quote_sprite.size[0])//2, SCREEN_HEIGHT-quote_sprite.size[1]-20)


    c = ClockEntity(world, 150, 150)


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

