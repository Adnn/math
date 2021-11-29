# Parameter Animation design document

## Combinations

| Result type | Easing    | Periodicity | Speed/Period | Period is max output | S distinct from P | remultiply | normalize | is finite |
| ----------- | --------- | ----------- | ------------ | -------------------- | ----------------- | ---------- | --------- | --------- |
| Full-range  | No-ease   | No-period   | S            | N                    | N                 | N          | N         | N         |
| Clamped     | No-ease   | No-period   | S or P       | N                    | N                 | N          | Y         | Y         |
| Full-range  | Nonlinear | No-period   | S+P          | Y                    | Y                 | Y          | Y         | Y         |
| Clamped     | Nonlinear | No-period   | P            | N                    | N                 | N          | Y         | Y         |
| Full-range  | No-ease   | Periodic    | S+P          | Y                    | Y                 | N          | N         | N         |
| Clamped     | No-ease   | Periodic    | P            | N                    | N                 | N          | Y         | N         |
| Full-range  | Nonlinear | Periodic    | S+P          | Y                    | Y                 | Y          | Y         | N         |
| Clamped     | Nonlinear | Periodic    | P            | N                    | N                 | N          | Y         | N         |


## Initial thought process

The period control the max output value, while the speed controls at which rate we get there.
It is not necessarily interchangeable.
But in clamped cases, the max value is known in advance, and this introduces coupling.


### Simplest use case

The simplest case requires to to advance the parameter linearly & monotonously
* Parameter origin value (allow for local animation time)
* Optional speed

### Lerp between two values
* Lerp expects a parameter input clamped to [0 1] (otherwise, this is extrapolation)
  For this reason, the parameter animation output range must be normalized to [0 1]

### Input param linearization:
* Implemented as `current_value / max_value` (then clamping)


### Combinatorics:
* Result type (i.e. parameter output range): Full-range or Clamped (Clamped is usefull for lerp())
* Easing: No-easing (== linear, no need to normalize input param) / Nonlinear (usually require input normalization)
* Periodicity: Unlimited (== no period) or Periodic (Repeat, PingPong)
* (Direction inversion: probably best implemented via negative speed)


#### No need for duration (or generally parameter input max value) when:
* Full-range / No-ease / Unlimited -> trivial case. There is no max value.

**Attention**
* Clamped / No-ease / Unlimited -> "ill" case, it does not require input max value per-se, as the output will just be input * speed.
But for this output to make sense, that speed will likely always be computed as 1/input_max_value -> it normalizes the input

#### Full-range / No-ease / Periodic
Does not require input normalization, as there is no-easing. But periodic needs the max_value anyway.
It also makes sense to have a speed to control the animation.
Speed and max_value are totally separate in this case, in this situation max_value will also be the output max_value.

#### Full-range / Nonlinear / *
* Unlimited: input normalization requires max_value, once input reaches its max value then will always clamp output to 1.
* Periodic: input normalization requires max_value, so output will always stays in [0..1] (at least for all easing functions I could think of). Period also requires max_value.

**Attention**
This is not what is needed, probably need to re-multiply the [0 1] output from `ease()` by max value.

#### Clamped / Nonlinear / Unlimited
Input normalization for easing, so output will always stays in [0..1] (for all easing functions I could think of) -> it will stay at 1 once input max_value is reached.

#### Clamped / No-ease / Periodic
Requires duration for Periodic behaviour, but no input normalization per-se since there is not easing (see note below).

**Attention**
For this output to make sense, the speed will likely always be computed as 1/input_max_value -> normalizes the input.
This case was naturally adressed by the previous implementation, since it normalized the input value even for Linear (==No-ease) easing.

#### Clamped / Nonlinear / Periodic
Require input normalization, but this case would indicate that the normalization is applied to the output of the behaviour (not before the periodic behaviour).


