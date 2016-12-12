#!/usr/bin/python3

import datetime
import math
import sys
import sdl2
import sdl2.ext
import select
import time
from enum import Enum

RESOURCES = sdl2.ext.Resources(__file__, 'resources')
SCREEN_WIDTH = 900  # 270
SCREEN_HEIGHT = 1600  # 480
HPADDING = 40
VPADDING = 40
HR_WIDTH = 1
NOTIF_HEIGHT = 268

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
                'screen': Screen.camera_1,
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


class TimerUpdater(sdl2.ext.Applicator):

    def __init__(self):
        super(TimerUpdater, self).__init__()
        self.componenttypes = (DynamicSprite, Timer,)
        self.fontmanager = sdl2.ext.FontManager(
                RESOURCES.get_path('Roboto-Regular.ttf'),
                )
        self.factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)

    def process(self, world, componentsets):
        global current_screen
        for ds, timer in componentsets:
            # handle state
            if timer.duration == 120:
                if current_screen == Screen.timer_1:
                    if timer.is_done():
                        current_screen = Screen.timer_2
                    elif not timer.running:
                        timer.restart()
                elif current_screen == Screen.timer_2:
                    pass
                else:
                    timer.reset()
                    ds.is_new = True
            elif timer.duration == 5:
                if current_screen == Screen.camera_1:
                    if timer.is_done():
                        current_screen = Screen.camera_2
                    elif not timer.running:
                        timer.restart()
                elif current_screen == Screen.camera_2:
                    pass
                else:
                    timer.reset()
                    ds.is_new = True
            # handle update
            if timer.update() or ds.is_new:
                if timer.duration == 5 and not timer.time_left:
                    break
                elif timer.duration == 5 and timer.time_left == 1:
                    print('photo')
                    sys.stdout.flush()
                position = ds.sprite.position
                ds.is_new = False
                ds.sprite = self.factory.from_text(
                        str(timer.time_left) if timer.duration < 60 else timer.get_time_left(),
                        size = ds.text_size,
                        fontmanager=self.fontmanager
                        )
                ds.sprite.position = (SCREEN_WIDTH-ds.sprite.size[0])//2, position[1]


class NotificationUpdater(sdl2.ext.Applicator):

    def __init__(self):
        super(NotificationUpdater, self).__init__()
        self.componenttypes = (DynamicSprite, Notification,)
        self.fontmanager = sdl2.ext.FontManager(
                RESOURCES.get_path('Roboto-Regular.ttf'),
                )
        self.factory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)

    def process(self, world, componentsets):
        global current_screen
        for ds, notif in componentsets:
            # handle update
            notif.update()
            if ds.is_new:
                notif.restart()
                position = ds.sprite.position
                ds.is_new = False
                if notif.type_ == 'app':
                    ds.sprite = self.factory.from_text(
                            '[{}]'.format(notif.text),
                            size = 48,
                            fontmanager=self.fontmanager
                            )
                    ds.sprite.position = (HPADDING + 40, 520)
                elif notif.type_ == 'title':
                    ds.sprite = self.factory.from_text(
                            notif.text,
                            size = 40,
                            fontmanager=self.fontmanager
                            )
                    ds.sprite.position = (HPADDING + 40, 630)
                elif notif.type_ == 'subtitle':
                    ds.sprite = self.factory.from_text(
                            notif.text,
                            size = 32,
                            fontmanager=self.fontmanager
                            )
                    ds.sprite.position = (HPADDING + 40, 690)
                elif notif.type_ == 'vl':
                    ds.sprite = self.factory.from_color(
                            sdl2.ext.Color(255, 255, 255),
                            (2, NOTIF_HEIGHT),
                            )
                    ds.sprite.position = (HPADDING, 500)
                elif notif.type_ == 'vr':
                    ds.sprite = self.factory.from_color(
                            sdl2.ext.Color(255, 255, 255),
                            (2, NOTIF_HEIGHT),
                            )
                    ds.sprite.position = (SCREEN_WIDTH - HPADDING - 2, 500)
                elif notif.type_ == 'ht':
                    ds.sprite = self.factory.from_color(
                            sdl2.ext.Color(255, 255, 255),
                            (SCREEN_WIDTH - HPADDING - HPADDING, 2),
                            )
                    ds.sprite.position = (HPADDING, 500)
                elif notif.type_ == 'hm':
                    ds.sprite = self.factory.from_color(
                            sdl2.ext.Color(255, 255, 255),
                            (SCREEN_WIDTH - HPADDING - HPADDING, 1),
                            )
                    ds.sprite.position = (HPADDING, 610)
                elif notif.type_ == 'hb':
                    ds.sprite = self.factory.from_color(
                            sdl2.ext.Color(255, 255, 255),
                            (SCREEN_WIDTH - HPADDING - HPADDING, 2),
                            )
                    ds.sprite.position = (HPADDING, 500 + NOTIF_HEIGHT - 2)



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


class TimerEntity(sdl2.ext.Entity):

    def __init__(self, world, time, *screens, size=16, posx=0, posy=0):
        self.timer = Timer(time)
        self.dynamicsprite = DynamicSprite(size)
        self.dynamicsprite.sprite.position = posx, posy
        self.itemset = ScreenSet(list(screens))


class NotificationEntity(sdl2.ext.Entity):

    def __init__(self, world, type_, text='', *screens, posx=0, posy=0):
        self.notification = Notification(type_, text)
        self.dynamicsprite = DynamicSprite()
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


class Timer(object):

    def __init__(self, duration):
        super(Timer, self).__init__()
        self.duration = duration
        self.reset()

    def restart(self):
        self.target = time.time() + self.duration
        self.lastdiff = self.target - time.time()
        self.running = True

    def reset(self):
        self.time_left = self.duration
        self.running = False

    def is_done(self):
        return not self.time_left

    def update(self):
        if self.running:
            now = time.time()
            if self.target < now:
                self.time_left = 0
                self.running = False
                return True
            elif int(self.lastdiff) - int(self.target - now):
                self.lastdiff = self.target - now
                self.time_left = math.ceil(self.target - now)
                return True
        return False

    def get_time_left(self):
        return '{:1d}:{:02d}'.format(self.time_left//60, self.time_left%60)


class Notification(object):

    def __init__(self, type_, text):
        super(Notification, self).__init__()
        self.duration = 2
        self.type_ = type_
        self.text = text
        self.reset()

    def restart(self):
        self.target = time.time() + self.duration
        self.lastdiff = self.target - time.time()
        self.running = True

    def reset(self):
        self.time_left = self.duration
        self.running = False

    def is_done(self):
        return not self.time_left

    def update(self):
        if self.running:
            now = time.time()
            if self.target < now:
                self.time_left = 0
                self.running = False
                return True
            elif int(self.lastdiff) - int(self.target - now):
                self.lastdiff = self.target - now
                self.time_left = math.ceil(self.target - now)
        return False

    def get_time_left(self):
        return '{:1d}:{:02d}'.format(self.time_left//60, self.time_left%60)


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
        return (SCREEN_WIDTH-size[0])//2, SCREEN_HEIGHT-240-size[1]
    if direction == 'down':
        return (SCREEN_WIDTH-size[0])//2, SCREEN_HEIGHT-90
    if direction == 'left':
        return SCREEN_WIDTH//2-80-size[0], SCREEN_HEIGHT-160-size[1]//2
    if direction == 'right':
        return SCREEN_WIDTH//2+80, SCREEN_HEIGHT-160-size[1]//2


def create_notif(world, app, title, subtitle):
    notif = {}
    notif['app'     ] = NotificationEntity(world, 'app'     , app, Screen.home)
    notif['title'   ] = NotificationEntity(world, 'title'   , title, Screen.home)
    notif['subtitle'] = NotificationEntity(world, 'subtitle', subtitle, Screen.home)
    notif['vl'      ] = NotificationEntity(world, 'vl'      , '', Screen.home)
    notif['vr'      ] = NotificationEntity(world, 'vr'      , '', Screen.home)
    notif['ht'      ] = NotificationEntity(world, 'ht'      , '', Screen.home)
    notif['hm'      ] = NotificationEntity(world, 'hm'      , '', Screen.home)
    notif['hb'      ] = NotificationEntity(world, 'hb'      , '', Screen.home)
    return notif


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
    du = DateUpdater()
    ttu = TimerUpdater()
    nu = NotificationUpdater()

    world.add_system(tu)
    world.add_system(du)
    world.add_system(ttu)
    world.add_system(nu)


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

    entities['arrow_center'] = StaticEntity(world, sprites['arrow_center'], (SCREEN_WIDTH - 70) // 2, SCREEN_HEIGHT - 200)
    entities['arrow_u_f'] = ScreenEntity(world, sprites['arrow_u_f'], posx=sprites['arrow_center'].position[0]   , posy=sprites['arrow_center'].position[1]-25)
    entities['arrow_u_e'] = ScreenEntity(world, sprites['arrow_u_e'], posx=sprites['arrow_center'].position[0]   , posy=sprites['arrow_center'].position[1]-25)
    entities['arrow_d_f'] = ScreenEntity(world, sprites['arrow_d_f'], posx=sprites['arrow_center'].position[0]   , posy=sprites['arrow_center'].position[1]+70)
    entities['arrow_d_e'] = ScreenEntity(world, sprites['arrow_d_e'], posx=sprites['arrow_center'].position[0]   , posy=sprites['arrow_center'].position[1]+70)
    entities['arrow_l_f'] = ScreenEntity(world, sprites['arrow_l_f'], posx=sprites['arrow_center'].position[0]-25, posy=sprites['arrow_center'].position[1]   )
    entities['arrow_l_e'] = ScreenEntity(world, sprites['arrow_l_e'], posx=sprites['arrow_center'].position[0]-25, posy=sprites['arrow_center'].position[1]   )
    entities['arrow_r_f'] = ScreenEntity(world, sprites['arrow_r_f'], posx=sprites['arrow_center'].position[0]+70, posy=sprites['arrow_center'].position[1]   )
    entities['arrow_r_e'] = ScreenEntity(world, sprites['arrow_r_e'], posx=sprites['arrow_center'].position[0]+70, posy=sprites['arrow_center'].position[1]   )

    for screen in Screen:
        entities['arrow_u_f'].screenset.items += [screen,] if     SCREEN_INFO[screen]['up'   ]['text'] else []
        entities['arrow_u_e'].screenset.items += [screen,] if not SCREEN_INFO[screen]['up'   ]['text'] else []
        entities['arrow_d_f'].screenset.items += [screen,] if     SCREEN_INFO[screen]['down' ]['text'] else []
        entities['arrow_d_e'].screenset.items += [screen,] if not SCREEN_INFO[screen]['down' ]['text'] else []
        entities['arrow_l_f'].screenset.items += [screen,] if     SCREEN_INFO[screen]['left' ]['text'] else []
        entities['arrow_l_e'].screenset.items += [screen,] if not SCREEN_INFO[screen]['left' ]['text'] else []
        entities['arrow_r_f'].screenset.items += [screen,] if     SCREEN_INFO[screen]['right']['text'] else []
        entities['arrow_r_e'].screenset.items += [screen,] if not SCREEN_INFO[screen]['right']['text'] else []

    for screen in Screen:
        for dir_ in ('up', 'down', 'left', 'right',):
            if not SCREEN_INFO[screen][dir_]['text']:
                continue
            s = factory.from_text(SCREEN_INFO[screen][dir_]['text'], size=32, fontmanager=fm)
            x, y = arrow_pos(s.size, dir_)
            e = ScreenEntity(world, to_enable_sprite(s), posx=x, posy=y)
            e.itemset.items += [screen,]

    entities['clock'] = ClockEntity(world, size=120, posx=40, posy=200)
    entities['calendar'] = CalendarEntity(world, size=36, posx=40, posy=360)
    entities['tb_timer'] = TimerEntity(world, 120, size=120, posx=10, posy=500)
    entities['cam_timer'] = TimerEntity(world, 5, size=240, posx=0, posy=300)

    entities['clock'].screenset.items = [Screen.home]
    entities['calendar'].screenset.items = [Screen.home]
    entities['tb_timer'].screenset.items = [Screen.timer_0, Screen.timer_1, Screen.timer_2]
    entities['cam_timer'].screenset.items = [Screen.camera_0, Screen.camera_1]

    # Text entities
    s = factory.from_text('Taking photo in...', size=48, fontmanager=fm)
    (x, y) = ((SCREEN_WIDTH-s.size[0])//2, 240)
    entities['cam_timer_1'] = ScreenEntity(world, to_enable_sprite(s), posx=x, posy=y)
    entities['cam_timer_1'].itemset.items += [Screen.camera_1,]

    s = factory.from_text('Photo taken!', size=48, fontmanager=fm)
    (x, y) = ((SCREEN_WIDTH-s.size[0])//2, 420)
    entities['cam_timer_2'] = ScreenEntity(world, to_enable_sprite(s), posx=x, posy=y)
    entities['cam_timer_2'].itemset.items += [Screen.camera_2,]

    notif = create_notif(world, 'AppName', 'Example Title', 'subtitle goes here')


    running = True
    while running:
        if select.select([sys.stdin,],[],[],0.0)[0]:
            message = next(sys.stdin).strip()
            if message == 'usb-in':
                current_state[State.usb] = True
            elif message == 'usb-out':
                current_state[State.usb] = False
            elif message == 'g:up':
                current_screen = SCREEN_INFO[current_screen]['up'   ]['screen']
            elif message == 'g:down':
                if current_screen == Screen.home:
                    print('lights')
                    sys.stdout.flush()
                    current_state[State.light] = not current_state[State.light]
                current_screen = SCREEN_INFO[current_screen]['down' ]['screen']
            elif message == 'g:left':
                current_screen = SCREEN_INFO[current_screen]['left' ]['screen']
            elif message == 'g:right':
                current_screen = SCREEN_INFO[current_screen]['right']['screen']
            else:
                print(message)
                sys.stdout.flush()
        if notif and not notif['app'].notification.time_left:
            world.delete_entities(notif.values())
            notif = {}
        events = sdl2.ext.get_events()
        for event in events:
            if event.type == sdl2.SDL_QUIT:
                running = False
                break
            if event.type == sdl2.SDL_KEYUP:
                if event.key.keysym.sym == sdl2.SDLK_q:
                    running = False
                    break
                if event.key.keysym.sym == sdl2.SDLK_s:
                    current_state[State.water] = not current_state[State.water]
                if event.key.keysym.sym == sdl2.SDLK_d:
                    current_state[State.images] = not current_state[State.images]
                if event.key.keysym.sym == sdl2.SDLK_UP:
                    current_screen = SCREEN_INFO[current_screen]['up'   ]['screen']
                if event.key.keysym.sym == sdl2.SDLK_DOWN:
                    if current_screen == Screen.home:
                        print('lights')
                        sys.stdout.flush()
                        current_state[State.light] = not current_state[State.light]
                    current_screen = SCREEN_INFO[current_screen]['down' ]['screen']
                if event.key.keysym.sym == sdl2.SDLK_LEFT:
                    current_screen = SCREEN_INFO[current_screen]['left' ]['screen']
                if event.key.keysym.sym == sdl2.SDLK_RIGHT:
                    current_screen = SCREEN_INFO[current_screen]['right']['screen']
        world.process()
    sdl2.ext.quit()

if __name__ == '__main__':
    sys.exit(run())

