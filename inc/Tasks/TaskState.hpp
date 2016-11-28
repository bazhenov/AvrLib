#pragma once

#include "HAL/Atmel/SleepMode.hpp"
#include "Time/RealTimer.hpp"
#include "Time/Units.hpp"
#include "Option.hpp"

namespace Impl {

template <typename time_t>
class TaskState {
	Option<time_t> tLeft;
	HAL::Atmel::SleepMode maxSleepMode;
public:
	/**
	 * Returns the deepest sleep mode this task can allow the system to go into, when the task
	 * is not idle.
	 */
	HAL::Atmel::SleepMode getMaxSleepMode() const { return maxSleepMode; }

	constexpr TaskState(Option<time_t> t, HAL::Atmel::SleepMode s): tLeft(t), maxSleepMode(s) {}

	/**
	 * Returns whether this task is currently idle.
	 */
	constexpr bool isIdle() const { return tLeft.isEmpty(); }

	/**
	 * For non-idle tasks, returns the amount of time until the task will reach its next deadline or timeout.
	 * This method must not be called for idle tasks.
	 * Any task defines this; for unknown run-times, the task should then implement and return
	 * a reasonable timeout.
	 */
	constexpr time_t timeLeft() const { return tLeft.get(); }
};

}

/**
 * Returns a TaskState from the given Deadline instance, marking the task as idle
 * if the deadline isn't currently scheduled.
 */
template <typename rt_t, typename value, typename check>
constexpr Impl::TaskState<Time::Counts<>> TaskState(Time::Deadline<rt_t,value,check> t, HAL::Atmel::SleepMode s) {
	if (t.isScheduled()) {
		return { some(t.timeLeft().template toCountsOn<rt_t>()), s };
	} else {
		return { none(), s };
	}
}

/**
 * Returns a TaskState from the given VariableDeadline instance, marking the task as idle
 * if the deadline isn't currently scheduled.
 */
template <typename rt_t>
constexpr Impl::TaskState<Time::Counts<>> TaskState(Time::VariableDeadline<rt_t> t, HAL::Atmel::SleepMode s) {
	if (t.isScheduled()) {
		return { some(t.timeLeft().template toCountsOn<rt_t>()), s };
	} else {
		return { none(), s };
	}
}

/**
 * Returns a TaskState indicating a non-idle task, which will run for [time] allowing sleep mode [s].
 * FIXME put in a check that it's actually a time unit
 */
template <typename time_t>
constexpr Impl::TaskState<time_t> TaskState(time_t time, HAL::Atmel::SleepMode s) {
	return { some(time), s };
}

/**
 * Returns a TaskState indicating an idle task, that allows for the system to go to full shutdown.
 * FIXME put in a check that it's actually a time unit
 */
template <typename time_t>
constexpr Impl::TaskState<time_t> TaskStateIdle() {
	return Impl::TaskState<time_t>(none(), HAL::Atmel::SleepMode::POWER_DOWN);
}
