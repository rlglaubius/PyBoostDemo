#include "pch.h"
#include "SIR.H"

SIR::SIR(const size_t first_time, const size_t final_time) :
	_first_time(first_time),
	_final_time(final_time),
	_num_units(final_time - first_time + 1),
	_num_steps(10),
	_step_size(0.1),
	_rate_enter(-1),
	_rate_leave(-1),
	_rate_transmit(-1),
	_rate_recover(-1)
{
	_state = new Array2D(_num_units, NUM_STATES);
	_event = new Array2D(_num_units, NUM_EVENTS);
}

SIR::~SIR() {
	if (_state != NULL)
		delete _state;

	if (_event != NULL)
		delete _event;
}

void SIR::initialize(
	const double rate_enter,
	const double rate_leave,
	const double rate_transmit,
	const double rate_recover,
	const double num_susceptible,
	const double num_infected,
	const double num_recovered)
{
	rateEnter(rate_enter);
	rateLeave(rate_leave);
	rateTransmit(rate_transmit);
	rateRecover(rate_recover);

	_state->set(0, STATE_SUS, num_susceptible);
	_state->set(0, STATE_INF, num_infected);
	_state->set(0, STATE_REC, num_recovered);
}

void SIR::calculate() {
	for (size_t t(1); t < _num_units; ++t) {
		calculate(t);
	}
}

void SIR::calculate(size_t t) {
	double force, popsize;
	double dstate[NUM_STATES] = { 0.0, 0.0, 0.0 };
	double devent[NUM_EVENTS] = { 0.0, 0.0, 0.0, 0.0 };

	// This declares a reference named "state" to the object pointed to by
	// the pointer "_state". This lets us write "state(t,j)" instead of
	// "_state->get(t,j)"
	Array2D& state(*_state);

	// initialize state(t) to state(t-1) so that we can accumulate
	// state changes in-place
	for (int j(0); j < NUM_STATES; ++j)
		state(t, j) = state(t - 1, j);

	for (int j(0); j < NUM_EVENTS; ++j)
		_event->set(t, j, 0.0);

	for (int step(0); step < _num_steps; ++step) {
		popsize = state(t, STATE_SUS) + state(t, STATE_INF) + state(t, STATE_REC);
		force = _rate_transmit * state(t, STATE_INF) / popsize;
		dstate[STATE_SUS] = _rate_enter - (force + _rate_leave) * state(t, STATE_SUS);
		dstate[STATE_INF] = force * state(t, STATE_SUS) - (_rate_recover + _rate_leave) * state(t, STATE_INF);
		dstate[STATE_REC] = _rate_recover * state(t, STATE_INF) - _rate_leave * state(t, STATE_REC);

		for (int j(0); j < NUM_STATES; ++j)
			state(t, j) += dstate[j] * _step_size;

		devent[EVENT_ENTER] += _step_size * _rate_enter;
		devent[EVENT_LEAVE] += _step_size * _rate_leave * popsize;
		devent[EVENT_TRANSMIT] += _step_size * force * state(t, STATE_SUS);
		devent[EVENT_RECOVER] += _step_size * _rate_recover * state(t, STATE_INF);
	}

	for (int j(0); j < NUM_EVENTS; ++j)
		_event->set(t, j, devent[j]);
}
