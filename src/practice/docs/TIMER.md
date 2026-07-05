# Timer, Rendering Lag, Movement, and Lookdown

This document explains how GoldenEye's mission timer behaves, why rendering
lag costs time even though the timer compensates for dropped frames, and why
speedrunners look at the ground while travelling.

The first part describes the behaviour without requiring any knowledge of the
source code. A technical walkthrough with links to the implementation follows.

## Short version

- The mission timer is not a count of rendered pictures.
- When rendering falls behind, the next gameplay update normally adds all of
  the missed timer ticks at once.
- Bond's movement is not calculated as a simple constant speed multiplied by
  elapsed time. His forward movement is extracted from a walking animation,
  filtered, and then applied to his position.
- Forward and strafe inputs create separate, perpendicular movement
  components. The game does not normalise their sum, so holding both produces
  faster diagonal movement.
- Processing one update representing several elapsed ticks is not equivalent
  to processing each tick separately. The timer catches up exactly in normal
  lag, but movement does not.
- Looking down makes fewer rooms, objects, guards, effects, and polygons
  visible. This reduces the chance that rendering falls behind, so Bond gets
  more individual movement updates per timer second.
- Looking down does not directly alter Bond's movement speed or horizontal
  movement direction.

## Terminology

Several different things are casually called a "frame". Keeping them separate
is important:

- **Video retrace:** the television output starts another refresh. This
  continues at approximately 60 Hz in NTSC even if the game has not prepared a
  new picture.
- **Rendered frame:** a new picture submitted by the game.
- **Gameplay update:** one execution of game logic, including input, movement,
  AI, and the mission timer.
- **Timer tick:** one nominal 1/60-second unit used by the NTSC mission timer.
- **Dropped frame:** in this document, a video retrace for which the game could
  not perform another gameplay/render update.

The game can therefore output several video retraces while displaying the same
rendered frame. At the next gameplay update it is told how many nominal timer
ticks elapsed.

## How the mission timer works

### Converting wall time to timer ticks

The N64 has a continuously increasing CPU counter. On every accepted gameplay
update, the NTSC game calculates:

```text
elapsed_ticks = floor((elapsed_counter_cycles + 387,937) / 775,875)
```

Adding 387,937 (approximately half of 775,875) makes this division round to the
nearest tick rather than always rounding down.

For ordinary retrace-aligned updates this gives:

| Gameplay updates | Video intervals elapsed | `elapsed_ticks` |
| ---: | ---: | ---: |
| normal | 1 | 1 |
| one update missed | 2 | 2 |
| two updates missed | 3 | 3 |
| nine updates missed | 10 | 10 |

When the game is active and unpaused, the mission timer is increased by
`elapsed_ticks`. Consequently, a dropped frame does not normally make the
mission timer pause:

```text
normal:
    update        update
    timer +1      timer +1

one dropped update:
    update        no update       update
    timer +1                      timer +2
```

Both timelines add three ticks over three video intervals. The second timeline
only gives Bond two opportunities to move and the player two opportunities to
provide input.

### Is it exactly wall-clock time?

No. It is an approximation with several sources of difference:

1. It is quantised to 1/60-second units.
2. Elapsed time is rounded independently at each gameplay update.
3. Its constants describe a nominal clock, not a calibrated real-time clock.
4. Physical console oscillators have tolerances, and emulators can implement
   timing differently.
5. The timer is deliberately stopped while the game is paused, controls are
   locked, or the current camera/game state says the mission timer is inactive.
6. Practice-ROM timescale and pause features deliberately modify the elapsed
   tick count.

At normal NTSC cadence the timer generally advances by one tick per video
retrace. The configured video mode has a nominal rate of approximately:

```text
2 × 48,681,812 / (3,093 × 525)
    = 59.9594316 retraces per real second
```

The timer labels 60 ticks as one second, so at this cadence its rate is:

```text
59.9594316 / 60
    = 0.99932386 displayed seconds per real second
```

Using those nominal hardware constants, the timer loses approximately:

| Real duration | Timer loss |
| ---: | ---: |
| 1 minute | 0.0406 seconds |
| 10 minutes | 0.406 seconds |
| 1 hour | 2.43 seconds |

This is the normal, frequent-update case. Rounding makes a pathological long
stall behave differently. If updates become extremely sparse, the rounding
error at each update becomes negligible and the conversion tends towards:

```text
46,875,000 counter cycles per second / 775,875 cycles per tick
    = 60.4156597 timer ticks per real second

60.4156597 / 60
    = 1.00692766 displayed seconds per real second
```

In that limiting case the timer would gain about 0.416 seconds per minute.
This is not the behaviour of normal one-to-several-frame lag. With the nominal
NTSC timing, batches of up to about 65 retraces still round to the same number
of ticks as retraces; a single batch of approximately 66 retraces crosses an
extra-tick rounding boundary. The timer's precise drift is therefore
update-cadence dependent.

### Display precision

The underlying NTSC timer is an integer tick count. The practice display
calculates:

```text
minutes    = ticks / 3,600
seconds    = (ticks / 60) mod 60
hundredths = floor((ticks mod 60) × 100 / 60)
```

The displayed hundredths are therefore derived from sixtieths and truncated.
They are not measurements from a hundredth-second clock. For example, the
sequence begins `.00`, `.01`, `.03`, `.05`, rather than containing every
hundredth.

The original results screen normally discards the fractional second entirely.

### PAL

PAL uses the same design with 50 Hz raw ticks:

```text
raw_elapsed_ticks = floor((elapsed_counter_cycles + 465,525) / 931,050)
```

When read for mission timing, the accumulated raw timer is converted to 60 Hz
units using integer arithmetic:

```text
reported_ticks = floor(raw_timer × 60 / 50)
```

The same conclusions apply: ordinary dropped updates are caught up, displayed
time is quantised, and rounding makes exact wall-clock correspondence
impossible.

## Why looking down is faster

### What lookdown changes

Looking down points the camera frustum mostly into nearby floor geometry.
Compared with looking horizontally across a level, this usually exposes:

- fewer rooms through the portal system;
- fewer guards and objects;
- fewer character models and animations;
- fewer particles, smoke clouds, bullet effects, and transparent surfaces;
- fewer background polygons for the Reality Signal Processor and Reality
  Display Processor to process.

This reduces both CPU work needed to decide and prepare what is visible and
graphics-processor work needed to draw it.

The game permits two graphics tasks to be outstanding. If both slots are
occupied when another video retrace arrives, it does not run another gameplay
and render update. The mission timer does not simply lose that time. On the
next accepted gameplay update, its elapsed-tick delta normally includes the
missed retrace. For example, after one skipped update the mission timer
advances by two ticks at once.

Bond does not receive two normal movement updates in that example. He receives
one movement update representing two elapsed ticks. As described below, that
batched animation and movement calculation is generally not equivalent to two
separate one-tick calculations and produces less forward progress on average.
The result is less distance travelled for the same increase in the mission
timer:

```text
normal:
    mission timer: +1 tick, then +1 tick
    movement:      two separately sampled movement updates

one update skipped:
    mission timer: +2 ticks
    movement:      one batched movement update
```

This distinction is why lookdown matters to an individual-level speedrun timed
by the in-game mission timer. It is not merely making the run finish sooner in
wall-clock time. By reducing visible work, lookdown avoids some skipped
gameplay updates and allows Bond to travel farther per mission-timer tick.

The camera's vertical angle is not used as a direct multiplier for horizontal
movement. Its benefit is indirect: less visible work means fewer batched
updates, and therefore less movement lost while the mission timer continues to
advance.

## Why lag reduces movement

### Movement is animation-driven

GoldenEye does not calculate ordinary forward movement as:

```text
distance = fixed_running_speed × elapsed_time
```

Bond has an animated first-person walking body. The game advances that
animation, extracts its horizontal root translation, smooths the result, and
uses the smoothed translation to move Bond. This is why the view sways as he
walks and why his displacement varies through the walking cycle.

For a normal one-tick update the process is conceptually:

```text
advance animation by one tick
sample this part of the stride
update the movement smoothing filter
move using the new filtered value
perform collision detection
```

If one update is dropped, the next update represents two elapsed ticks:

```text
advance animation by two ticks
sample the combined/endpoint animation displacement once
update the smoothing filter for two ticks using that one sample
move once using the final filtered value
perform collision detection once
```

The second operation cannot reproduce all the intermediate values from the
first. It knows the elapsed time, but it no longer has a distinct root-motion
sample, filtered velocity, input sample, or collision result for each skipped
update.

### How strafing joins the walking animation

Forward and strafe input are stored independently. Each is initially limited
to the range `-1` to `1`. Forward input then receives a `1.08` multiplier and
can receive an additional running boost; strafe input does not receive those
two forward-only multipliers.

The game uses approximately the following value to choose the walking
animation and its playback speed:

```text
animation_speed =
    max(
        abs(forward),
        0.8 × abs(strafe),
        0.8 × abs(turning)
    )
```

This is a maximum, not the length of the combined movement vector. Adding
strafe while already running forwards therefore does not reduce or normalise
the forward component.

Forward displacement comes from the walking animation's forward root
translation. Strafe displacement is added to the animation's sideways root
translation before smoothing:

```text
added_strafe_translation =
    strafe
    × sprint_root_speed
    × 0.5
    × elapsed_ticks
```

`sprint_root_speed` is measured from the sprint animation when the player is
initialised. The later division by `elapsed_ticks` turns the added strafe
translation back into a per-tick value. A sustained strafe therefore supplies
an almost constant sideways sample, while forward root motion still varies
with walking-animation phase.

After filtering, the local forward and sideways components are rotated by
Bond's horizontal facing direction and added:

```text
world_velocity =
    forward_component × facing_forward
    + strafe_component × facing_right
```

These basis vectors are perpendicular, so rotation does not alter the
magnitude:

```text
horizontal_speed =
    sqrt(forward_component² + strafe_component²)
```

If the two components are equal, the result is:

```text
sqrt(1² + 1²) = sqrt(2) ≈ 1.414
```

That is the familiar theoretical 41.4% diagonal speed increase. GoldenEye's
actual ratio is not always exactly `sqrt(2)` because forward and strafe use
different scaling, forward has the `1.08` and running-boost multipliers, and
the animation and smoothing state vary.

To travel straight along a corridor while speed-strafing, a runner faces
diagonally so that the vector sum of Bond's local forward and sideways
movement points along the corridor.

### Movement pseudocode

The following pseudocode shows the relevant sequence for ordinary horizontal
movement. It leaves out leaning, damage boosts, forced movement, slopes, and
most collision details.

`elapsed_ticks` is normally `1`. If one gameplay update was dropped, the next
update normally receives `2`; after two dropped updates it receives `3`, and
so on.

```text
function update_bond_movement(elapsed_ticks):
    # Input is sampled once for this entire update.
    forward = read_forward_input()
    strafe  = read_strafe_input()
    turning = read_turn_input()

    forward = clamp(forward, -1, 1)
    strafe  = clamp(strafe,  -1, 1)

    # These multipliers apply only to forward movement.
    forward = forward * 1.08
    forward = forward * running_speed_boost

    # This controls animation playback. It does not normalise the final
    # forward-plus-strafe movement vector.
    animation_speed = maximum(
        absolute(forward),
        0.8 * absolute(strafe),
        0.8 * absolute(turning)
    )

    set_walking_animation_speed(animation_speed)

    # The animation is advanced once by the entire elapsed interval.
    # Intermediate animation poses are not separately evaluated.
    advance_walking_animation(elapsed_ticks)
    animation_forward, animation_sideways = evaluate_root_translation()

    if animation_speed is not zero:
        animation_forward =
            animation_forward * (forward / animation_speed)

    # Strafing is explicitly inserted into the animation's sideways
    # translation. sprint_root_speed was measured from the sprint animation
    # when the player was initialised.
    animation_sideways +=
        strafe
        * sprint_root_speed
        * 0.5
        * elapsed_ticks

    # Convert the combined translation to a per-tick sample.
    forward_sample  = animation_forward  / elapsed_ticks
    sideways_sample = animation_sideways / elapsed_ticks

    forward_sample  *= animation_amplitude
    sideways_sample *= animation_amplitude

    # Advance the smoothing state by the elapsed number of ticks.
    # The important detail is that every iteration uses the same samples
    # obtained from the single animation evaluation above.
    repeat elapsed_ticks times:
        smoothed_forward_sum =
            0.93 * smoothed_forward_sum + forward_sample

        smoothed_sideways_sum =
            0.93 * smoothed_sideways_sum + sideways_sample

    filtered_forward  = 0.07 * smoothed_forward_sum
    filtered_sideways = 0.07 * smoothed_sideways_sum

    # Turn the per-tick values back into displacement for this whole update.
    local_forward_distance  = filtered_forward  * elapsed_ticks
    local_sideways_distance = filtered_sideways * elapsed_ticks

    # Rotate Bond's local axes into the level. No diagonal normalisation is
    # performed: both components survive and are added together.
    world_movement =
        local_forward_distance  * facing_forward
        + local_sideways_distance * facing_right

    perform_collision_and_move_once(world_movement)
```

For two uninterrupted ticks, the process is effectively:

```text
update_bond_movement(elapsed_ticks = 1)
    sample input
    evaluate animation phase A
    update smoothing
    move and collide

update_bond_movement(elapsed_ticks = 1)
    sample input again
    evaluate animation phase B
    update smoothing
    move and collide again
```

If one update is dropped, the same two mission-timer ticks are handled like
this:

```text
update_bond_movement(elapsed_ticks = 2)
    sample input once
    advance past phases A and B
    evaluate the animation once
    reuse that one result for both smoothing iterations
    apply one combined movement
    collide once
```

Those paths are not equivalent. The batched path has lost the distinct input,
root-motion, smoothing-output, position, and collision state that would have
existed between the two ticks.

There is no fixed rule such as "one dropped frame makes Bond exactly X percent
slower." The result depends on:

- which walking-animation phases were crossed;
- the existing forward and sideways smoothing state;
- whether strafe input started, stopped, or reversed;
- whether input or facing direction would have changed;
- collision geometry, slopes, boosts, and leaning.

Sustained strafe is less sensitive than forward movement because its explicitly
added sideways sample is almost constant. Forward motion comes from
phase-dependent animation translation. Strafe transitions and the animation's
natural sideways sway remain lag-sensitive.

The batched result can occasionally be favourable at a particular animation
phase, but increased lag reduces forward progress on average. This is also why
moving a weapon-switch lag spike by one frame can produce a noticeably
different result in a tool-assisted run.

If animation velocity, input, smoothing state, and collisions were all
perfectly constant, batching would not matter. Actual GoldenEye movement does
not satisfy those conditions.

### Other effects of batched updates

The animation/filter mismatch is not the only difference:

- Controller input is sampled once rather than once per elapsed timer tick.
- Turning and changes of strafe direction have fewer intermediate updates.
- Collision detection receives one larger movement request rather than
  several smaller requests.
- Doors, guards, objects, and scripts may be updated using a mixture of
  elapsed-tick scaling and once-per-gameplay-update logic.

This is why lag can also affect cornering, door interactions, firing rates,
guard behaviour, and collision warps.

## Timer time versus movement time

The central speedrunning consequence can be summarised as:

```text
                       Normal          One update dropped
timer ticks added         1 + 1                 2
forward root samples      2                     1 combined sample
strafe input samples      2                     1
controller samples        2                     1
collision passes          2                     1
```

The mission timer knows that two ticks elapsed. Bond's movement system cannot
reconstruct the two gameplay states that were skipped. Looking down preserves
more of those individual updates, increasing distance travelled per displayed
mission second.

Community testing historically estimates the saving from sustained useful
lookdown at roughly one second per 75 seconds of movement, but this is not a
universal constant. The benefit depends on the level, camera direction,
visible rooms and characters, effects, current animation phase, and where lag
spikes occur. The historical community descriptions are archived in the
[Lookdown](https://archive.speedrunwiki.com/Lookdown) and
[Lag](https://archive.speedrunwiki.com/w/index.php?title=Lag) articles.

## Technical walkthrough

### 1. The main loop can skip gameplay updates

The main thread receives video-retrace messages. It only begins another update
when fewer than two graphics tasks are pending:

- [`src/boss.c`, main-loop retrace handling](../../boss.c#L456-L487)
- [`src/boss.c`, graphics task submission](../../boss.c#L611-L615)

When the condition fails, the saved CPU-counter timestamp is left untouched.
Elapsed time therefore accumulates until an update is accepted.

### 2. Elapsed CPU time becomes an integer delta

`waitForNextFrame` reads the CPU counter and performs the NTSC or PAL rounded
division:

- [`src/game/unk_0C0A70.c`, `waitForNextFrame`](../../game/unk_0C0A70.c#L102-L116)

`updateFrameCounters` saves the resulting value in `speedgraphframes`:

- [`src/game/unk_0C0A70.c`, `updateFrameCounters`](../../game/unk_0C0A70.c#L48-L75)

The declared nominal CPU counter rate is 46.875 MHz:

- [`include/PR/os.h`, N64 clock description](../../../include/PR/os.h#L696-L707)

The NTSC video-clock constant and configured horizontal/vertical timings used
in the wall-clock calculation above are defined here:

- [`include/PR/rcp.h`, `VI_NTSC_CLOCK`](../../../include/PR/rcp.h#L597-L601)
- [`src/libultra/io/vimodentsclan1.c`, NTSC VI mode](../../libultra/io/vimodentsclan1.c#L5-L18)

### 3. The delta advances the timer

Unless controls are locked or the game is paused, `speedgraphframes` becomes
`g_ClockTimer`:

- [`src/game/lvl.c`, clock update](../../game/lvl.c#L1281-L1299)

The first player's movement tick adds `g_ClockTimer` to `mission_timer` when
the current camera/timer state permits it:

- [`src/game/bondview.c`, mission timer increment](../../game/bondview.c#L11739-L11748)

PAL converts the raw 50 Hz timer to reported 60 Hz units here:

- [`src/game/bondview.c`, `getMissiontimer`](../../game/bondview.c#L20489-L20494)

The practice HUD converts those ticks to minutes, seconds, and hundredths:

- [`src/practice/practice_ui.c`, timer formatting](../practice_ui.c#L420-L431)

### 4. Camera direction determines visible work

Rendering first sets up the current player's camera, then determines visible
rooms and objects, builds the on-screen prop list, and renders the level:

- [`src/game/lvl.c`, `lvlRender`](../../game/lvl.c#L970-L1020)
- [`src/game/chrprop.c`, `chraiUpdateOnscreenPropCount`](../../game/chrprop.c#L188-L216)
- [`src/game/bg.c`, `bgLevelRender`](../../game/bg.c#L3633-L3651)

The vertical look angle is used to build the view matrix after the movement
animation update. It is not applied as a horizontal speed multiplier:

- [`src/game/bondview.c`, movement animation and view matrix](../../game/bondview.c#L8018-L8068)

### 5. Forward and strafe inputs remain separate

Digital and analogue controls populate independent `speedforwards` and
`speedsideways` values. Both are clamped independently, after which only
forward movement receives the `1.08` and running-boost multipliers:

- [`src/game/bondview.c`, forward and sideways input](../../game/bondview.c#L9360-L9434)
- [`src/game/bondview.c`, digital speed accumulation](../../game/bondview.c#L7661-L7722)

The game chooses animation speed using the maximum of scaled forward, strafe,
and turning magnitudes rather than the length of their combined vector:

- [`src/game/bondview.c`, `maxspeed` selection](../../game/bondview.c#L10927-L10962)

The strafe translation is calculated from the measured sprint root speed and
the elapsed tick delta:

- [`src/game/bondview.c`, strafe translation](../../game/bondview.c#L10897-L10900)
- [`src/game/initBondDATAdefaults.c`, measuring animation root speed](../../game/initBondDATAdefaults.c#L157-L167)

### 6. Movement comes through the animation matrices

`bondviewMoveAnimationTick` selects and updates Bond's movement animation:

- [`src/game/bondview.c`, `bondviewMoveAnimationTick`](../../game/bondview.c#L8018-L8056)

`bheadUpdate` advances the animation by the whole `g_ClockTimer` delta,
evaluates its matrices, adds the strafe translation to the local X axis,
scales the forward Z axis, and divides both by the elapsed delta:

- [`src/game/bondhead.c`, batched animation update](../../game/bondhead.c#L245-L289)

`bheadUpdatePos` applies the 0.93 smoothing recurrence once for every elapsed
tick, but every iteration uses the same translation sample from that one
evaluated animation state:

- [`src/game/bondhead.c`, position smoothing](../../game/bondhead.c#L70-L97)

Finally, `MoveBond` takes the resulting `headpos`, multiplies it by
`g_GlobalTimerDelta`, rotates the perpendicular local X/Z components into
world coordinates, and submits their sum to collision handling:

- [`src/game/bondview.c`, animation-derived movement](../../game/bondview.c#L10983-L11023)

That sequence is the source of the phase-dependent difference between several
separate one-tick updates and one multi-tick update.

## Conclusions

1. The timer is approximately wall-clock based, quantised to nominal 60 Hz
   ticks, and normally catches up after rendering lag.
2. Its exact long-term drift depends on hardware timing and how frequently the
   game accepts updates because each elapsed interval is rounded separately.
3. Looking down reduces visible CPU and graphics work; it does not directly
   modify Bond's speed.
4. Forward and strafe inputs are independent perpendicular components. Their
   vector sum is not normalised, which makes diagonal movement faster.
5. Both components pass through the animation matrices and smoothing filter,
   but sustained strafe supplies a more constant sample than forward root
   motion.
6. A multi-tick movement update is phase-dependent and is not equivalent to
   the skipped sequence of one-tick updates, even though both consume the same
   amount of mission-timer time.
7. Lookdown is faster because it reduces those lossy batched movement updates,
   allowing more distance to be travelled per mission-timer second.
