#!/usr/bin/python3

import datetime
import sys
import sdl2
import sdl2.ext
from enum import Enum

RESOURCES = sdl2.ext.Resources(__file__, 'resources')
SCREEN_WIDTH = 720  # 270
SCREEN_HEIGHT = 1280  # 480
HPADDING = 40
VPADDING = 40
HR_WIDTH = 1

SPRITE_NAMES = (
        'usb',
        'water',
        'light',
        'images',
        'arrow_center',
        'arrow_u_f',
        'arrow_u_e',
        'arrow_d_f',
        'arrow_d_e',
        'arrow_l_f',
        'arrow_l_e',
        'arrow_r_f',
        'arrow_r_e',
        )

FONT_COMP = {
        120: [41, 33],
        36: [12, 10],
        }

class Screen(Enum):
    home = 1
    camera_0 = 2
    camera_1 = 3
    camera_2 = 4
    graphs_0 = 5
    graphs_1 = 6
    graphs_2 = 7
    graphs_3 = 8
    timer_0 = 9
    timer_1 = 10
    timer_2 = 11

class State(Enum):
    usb = 1
    water = 2
    light = 3
    images = 4

SCREEN_INFO = {
        Screen.home: {
            'up': {
                'screen': Screen.camera_0,
                'text': 'Camera',
                },
            'down': {
                'screen': Screen.home,
                'text': 'Lights',
                },
            'left': {
                'screen': Screen.timer_0,
                'text': 'Timer',
                },
            'right': {
                'screen': Screen.graphs_0,
                'text': 'Water',
                },
            },
        Screen.camera_0: {
            'up': { 'screen': Screen.camera_0, 'text': '', },
            'down': {
                'screen': Screen.home,
                'text': 'Cancel',
                },
            'left': { 'screen': Screen.camera_0, 'text': '', },
            'right': { 'screen': Screen.camera_0, 'text': '', },
            },
        Screen.camera_1: {
            'up': { 'screen': Screen.camera_1, 'text': '', },
            'down': {
                'screen': Screen.home,
                'text': 'Cancel',
                },
            'left': { 'screen': Screen.camera_1, 'text': '', },
            'right': { 'screen': Screen.camera_1, 'text': '', },
            },
        Screen.camera_2: {
            'up': { 'screen': Screen.camera_2, 'text': '', },
            'down': {
                'screen': Screen.home,
                'text': 'Home',
                },
            'left': { 'screen': Screen.camera_2, 'text': '', },
            'right': { 'screen': Screen.camera_2, 'text': '', },
            },
        Screen.graphs_0: {
            'up': {
                'screen': Screen.graphs_1,
                'text': 'Weekly',
                },
            'down': { 'screen': Screen.graphs_0, 'text': '', },
            'left': {
                'screen': Screen.home,
                'text': 'Home',
                },
            'right': { 'screen': Screen.graphs_0, 'text': '', },
            },
        Screen.graphs_1: {
            'up': {
                'screen': Screen.graphs_2,
                'text': 'Monthly',
                },
            'down': {
                'screen': Screen.graphs_0,
                'text': 'Daily',
                },
            'left': {
                'screen': Screen.home,
                'text': 'Home',
                },
            'right': { 'screen': Screen.graphs_1, 'text': '', },
            },
        Screen.graphs_2: {
            'up': {
                'screen': Screen.graphs_3,
                'text': 'Yearly',
                },
            'down': {
                'screen': Screen.graphs_1,
                'text': 'Weekly',
                },
            'left': {
                'screen': Screen.home,
                'text': 'Home',
                },
            'right': { 'screen': Screen.graphs_2, 'text': '', },
            },
        Screen.graphs_3: {
            'up': { 'screen': Screen.graphs_3, 'text': '', },
            'down': {
                'screen': Screen.graphs_2,
                'text': 'Monthly',
                },
            'left': {
                'screen': Screen.home,
                'text': 'Home',
                },
            'right': { 'screen': Screen.graphs_3, 'text': '', },
            },
        Screen.timer_0: {
            'up': {
                'screen': Screen.timer_1,
                'text': 'Start',
                },
            'down': { 'screen': Screen.timer_0, 'text': '', },
            'left': { 'screen': Screen.timer_0, 'text': '', },
            'right': {
                'screen': Screen.home,
                'text': 'Home',
                },
            },
        Screen.timer_1: {
            'up': { 'screen': Screen.timer_1, 'text': '', },
            'down': { 'screen': Screen.timer_1, 'text': '', },
            'left': { 'screen': Screen.timer_1, 'text': '', },
            'right': {
                'screen': Screen.home,
                'text': 'Cancel',
                },
            },
        Screen.timer_2: {
            'up': { 'screen': Screen.timer_2, 'text': '', },
            'down': { 'screen': Screen.timer_2, 'text': '', },
            'left': { 'screen': Screen.timer_2, 'text': '', },
            'right': {
                'screen': Screen.home,
                'text': 'Home',
                },
            },
        }


# globals are fun, and useful!
current_screen = Screen.home
current_state = {
        State.usb: False,
        State.water: False,
        State.light: False,
        State.images: False,
        }



class SoftwareRenderer(sdl2.ext.SoftwareSpriteRenderSystem):

    def __init__(self, window):
        super(SoftwareRenderer, self).__init__(window)
        self.componenttypes = (EnableSprite,)#, DynamicSprite)

    def render(self, components):
        sdl2.ext.fill(self.surface, sdl2.ext.Color(0, 0, 0))
        super(SoftwareRenderer, self).render(self.select_sprites(components))

    def select_sprites(self, components):
        result = []
        for s in components:
            if not s.enabled:
                continue
            if type(s) is DynamicSprite:
                result.append(s.sprite)
            else:
                result.append(s)
        return result


class EnableUpdater(sdl2.ext.Applicator):

    def __init__(self):
        super(EnableUpdater, self).__init__()
        self.componenttypes = (EnableSprite, ItemSet,)

    def process(self, world, componentsets):
        global current_screen, current_state
        for sprite, item_set in componentsets:
            if type(item_set) is ScreenSet:
                sprite.enabled = current_screen in item_set.items
            if type(item_set) is StateSet:
                sprite.enabled = any([current_state[s] for s in item_set.items])


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

    def __init__(self, world, sprite, posx=0, posy=0):
        self.sprite = sprite
        self.sprite.position = (posx, posy,)


class ScreenEntity(sdl2.ext.Entity):

    def __init__(self, world, sprite, *screens, posx=0, posy=0):
        self.enablesprite = sprite
        self.sprite.position = (posx, posy,)
        self.itemset = ScreenSet(list(screens))


class StateEntity(sdl2.ext.Entity):

    def __init__(self, world, sprite, *states, posx=0, posy=0):
        self.enablesprite = sprite
        self.sprite.position = (posx, posy,)
        self.itemset = StateSet(list(states))


class ClockEntity(sdl2.ext.Entity):

    def __init__(self, world, *screens, size=16, posx=0, posy=0):
        self.clock = Clock()
        self.dynamicsprite = DynamicSprite(size)
        self.dynamicsprite.sprite.position = posx, posy
        self.itemset = ScreenSet(list(screens))


class CalendarEntity(sdl2.ext.Entity):

    def __init__(self, world, *screens, size=16, posx=0, posy=0):
        self.calendar = Calendar()
        self.dynamicsprite = DynamicSprite(size)
        self.dynamicsprite.sprite.position = posx, posy
        self.itemset = ScreenSet(list(screens))


# Helper classes

class EnableSprite(sdl2.ext.Sprite):

    def __init__(self, enabled=True):
        super(EnableSprite, self).__init__()
        self.enabled = enabled


class DynamicSprite(EnableSprite):

    def __init__(self, text_size=16, enabled=True):
        factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
        self.sprite = factory.from_color(sdl2.ext.Color(255,255,255), size=(1,1))
        self.is_new = True
        self.text_size = text_size
        self.enabled = enabled

    @property
    def depth(self):
        return self.sprite.depth


##  class SpriteGroup(sdl2.ext.Sprite):
##
##      def __init__(self, *sprite_info):
##          self.sprites = [s[0] for s in sprite_info]
##          self.positions = [p[1] for p in sprite_info]
##
##      @property
##      def depth(self):
##          return self.sprites[0].depth if self.sprites else 0
##
##      def set_pos(self, posx, posy):
##          for i in range(len(self.sprites)):
##              self.sprites[i].position = (
##                      posx + self.positions[i][0],
##                      posy + self.positions[i][1],
##                      )


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


class ItemSet(object):

    def __init__(self, items):
        super(ItemSet, self).__init__()
        self.items = items


class ScreenSet(ItemSet):

    def __init__(self, items):
        super(ScreenSet, self).__init__(items)


class StateSet(ItemSet):

    def __init__(self, items):
        super(StateSet, self).__init__(items)


# Convenience functions

def to_enable_sprite(sprite):
    sprite.__class__ = EnableSprite
    sprite.enabled = True
    return sprite


def arrow_pos(size, direction):
    if direction == 'up':
        return (SCREEN_WIDTH-size[0])//2, SCREEN_HEIGHT-840-size[1]
    if direction == 'down':
        return (SCREEN_WIDTH-size[0])//2, SCREEN_HEIGHT-690
    if direction == 'left':
        return SCREEN_WIDTH//2-80-size[0], SCREEN_HEIGHT-760-size[1]//2
    if direction == 'right':
        return SCREEN_WIDTH//2+80, SCREEN_HEIGHT-760-size[1]//2


# Main loop

def run():

    global current_screen, current_state

    sdl2.ext.init()
    window = sdl2.ext.Window('Magic Mirror', size=(SCREEN_WIDTH, SCREEN_HEIGHT))
    window.show()

    current_screen = Screen.home
    world = sdl2.ext.World()

    renderer = SoftwareRenderer(window)
    world.add_system(renderer)

    enable_updater = EnableUpdater()
    world.add_system(enable_updater)

    tu = TimeUpdater()
#   du = DateUpdater()

    world.add_system(tu)
#   world.add_system(du)


    factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)
    fm = sdl2.ext.FontManager(RESOURCES.get_path('Roboto-Regular.ttf'), size=32)

    sprites = {}

    # Load default sprites
    for name in SPRITE_NAMES:
        sprites[name] = factory.from_image(RESOURCES.get_path('{}.bmp'.format(name)))

    # Convert all to EnableSprites
    for key in sprites:
        sprites[key] = to_enable_sprite(sprites[key])


    entities = {}
    entities['usb']     = StateEntity(world, sprites['usb'],    posx=HPADDING,           posy=VPADDING)
    entities['water']   = StateEntity(world, sprites['water'],  posx=HPADDING * 2 + 80,  posy=VPADDING)
    entities['light']   = StateEntity(world, sprites['light'],  posx=HPADDING * 3 + 160, posy=VPADDING)
    entities['images']  = StateEntity(world, sprites['images'], posx=HPADDING * 4 + 240, posy=VPADDING)

    entities['usb'   ].stateset.items = [State.usb,]
    entities['water' ].stateset.items = [State.water,]
    entities['light' ].stateset.items = [State.light,]
    entities['images'].stateset.items = [State.images,]

    entities['arrow_center'] = StaticEntity(world, sprites['arrow_center'], (SCREEN_WIDTH - 70) // 2, SCREEN_HEIGHT - 800)
    entities['arrow_u_f'] = ScreenEntity(world, sprites['arrow_u_f'], posx=sprites['arrow_center'].position[0]   , posy=sprites['arrow_center'].position[1]-25)
    entities['arrow_u_e'] = ScreenEntity(world, sprites['arrow_u_e'], posx=sprites['arrow_center'].position[0]   , posy=sprites['arrow_center'].position[1]-25)
    entities['arrow_d_f'] = ScreenEntity(world, sprites['arrow_d_f'], posx=sprites['arrow_center'].position[0]   , posy=sprites['arrow_center'].position[1]+70)
    entities['arrow_d_e'] = ScreenEntity(world, sprites['arrow_d_e'], posx=sprites['arrow_center'].position[0]   , posy=sprites['arrow_center'].position[1]+70)
    entities['arrow_l_f'] = ScreenEntity(world, sprites['arrow_l_f'], posx=sprites['arrow_center'].position[0]-25, posy=sprites['arrow_center'].position[1]   )
    entities['arrow_l_e'] = ScreenEntity(world, sprites['arrow_l_e'], posx=sprites['arrow_center'].position[0]-25, posy=sprites['arrow_center'].position[1]   )
    entities['arrow_r_f'] = ScreenEntity(world, sprites['arrow_r_f'], posx=sprites['arrow_center'].position[0]+70, posy=sprites['arrow_center'].position[1]   )
    entities['arrow_r_e'] = ScreenEntity(world, sprites['arrow_r_e'], posx=sprites['arrow_center'].position[0]+70, posy=sprites['arrow_center'].position[1]   )

    for screen in Screen:
        entities['arrow_u_f'].screenset.items += [screen,] if SCREEN_INFO[screen]['up'   ]['screen'] is not screen else []
        entities['arrow_u_e'].screenset.items += [screen,] if SCREEN_INFO[screen]['up'   ]['screen'] is     screen else []
        entities['arrow_d_f'].screenset.items += [screen,] if SCREEN_INFO[screen]['down' ]['screen'] is not screen else []
        entities['arrow_d_e'].screenset.items += [screen,] if SCREEN_INFO[screen]['down' ]['screen'] is     screen else []
        entities['arrow_l_f'].screenset.items += [screen,] if SCREEN_INFO[screen]['left' ]['screen'] is not screen else []
        entities['arrow_l_e'].screenset.items += [screen,] if SCREEN_INFO[screen]['left' ]['screen'] is     screen else []
        entities['arrow_r_f'].screenset.items += [screen,] if SCREEN_INFO[screen]['right']['screen'] is not screen else []
        entities['arrow_r_e'].screenset.items += [screen,] if SCREEN_INFO[screen]['right']['screen'] is     screen else []

    for screen in Screen:
        for dir_ in ('up', 'down', 'left', 'right',):
            if not SCREEN_INFO[screen][dir_]['text']:
                continue
            s = factory.from_text(SCREEN_INFO[screen][dir_]['text'], size=32, fontmanager=fm)
            x, y = arrow_pos(s.size, dir_)
            e = ScreenEntity(world, to_enable_sprite(s), posx=x, posy=y)
            e.itemset.items += [screen,]

    print(s.enabled)

    entities['clock'] = ClockEntity(world, size=120, posx=10, posy=10)
    entities['calendar'] = CalendarEntity(world, size=36, posx=10, posy=200)

    entities['clock'].screenset.items = [Screen.home]
    entities['calendar'].screenset.items = [Screen.home]


    running = True
    while running:
        events = sdl2.ext.get_events()
        for event in events:
            if event.type == sdl2.SDL_QUIT:
                running = False
                break
            if event.type == sdl2.SDL_KEYUP:
                if event.key.keysym.sym == sdl2.SDLK_q:
                    running = False
                    break
                if event.key.keysym.sym == sdl2.SDLK_a:
                    current_state[State.usb] = not current_state[State.usb]
                if event.key.keysym.sym == sdl2.SDLK_s:
                    current_state[State.water] = not current_state[State.water]
                if event.key.keysym.sym == sdl2.SDLK_d:
                    current_state[State.light] = not current_state[State.light]
                if event.key.keysym.sym == sdl2.SDLK_f:
                    current_state[State.images] = not current_state[State.images]
                if event.key.keysym.sym == sdl2.SDLK_UP:
                    current_screen = SCREEN_INFO[current_screen]['up'   ]['screen']
                if event.key.keysym.sym == sdl2.SDLK_DOWN:
                    current_screen = SCREEN_INFO[current_screen]['down' ]['screen']
                if event.key.keysym.sym == sdl2.SDLK_LEFT:
                    current_screen = SCREEN_INFO[current_screen]['left' ]['screen']
                if event.key.keysym.sym == sdl2.SDLK_RIGHT:
                    current_screen = SCREEN_INFO[current_screen]['right']['screen']
        world.process()
    sdl2.ext.quit()

if __name__ == '__main__':
    sys.exit(run())

