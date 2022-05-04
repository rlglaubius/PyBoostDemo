#include "pch.h"
#include "SIR.H"

SIR::SIR(const size_t first_time, const size_t final_time, double* const work_state, double* const work_event) :
	_first_time(first_time),
	_final_time(final_time),
	_num_units(final_time - first_time + 1),
	_num_steps(10),
	_step_size(0.1),
	_rate_enter(-1),
	_rate_leave(-1),
	_rate_transmit(-1),
	_rate_recover(-1),
	_state(boost::multi_array_ref<double, 2>(work_state, boost::extents[final_time - first_time + 1][NUM_STATES])),
	_event(boost::multi_array_ref<double, 2>(work_event, boost::extents[final_time - first_time + 1][NUM_EVENTS]))
{}

SIR::~SIR() {}

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

	_state[0][STATE_SUS] = num_susceptible;
	_state[0][STATE_INF] = num_infected;
	_state[0][STATE_REC] = num_recovered;

	_event[0][EVENT_ENTER] = 0.0;
	_event[0][EVENT_LEAVE] = 0.0;
	_event[0][EVENT_TRANSMIT] = 0.0;
	_event[0][EVENT_RECOVER] = 0.0;
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

	// initialize state(t) to state(t-1) so that we can accumulate
	// state changes in-place
	for (int j(0); j < NUM_STATES; ++j)
		_state[t][j] = _state[t - 1][j];

	for (int j(0); j < NUM_EVENTS; ++j)
		_event[t][j] = 0.0;

	for (int step(0); step < _num_steps; ++step) {
		popsize = _state[t][STATE_SUS] + _state[t][STATE_INF] + _state[t][STATE_REC];
		force = _rate_transmit * _state[t][STATE_INF] / popsize;
		dstate[STATE_SUS] = _rate_enter - (force + _rate_leave) * _state[t][STATE_SUS];
		dstate[STATE_INF] = force * _state[t][STATE_SUS] - (_rate_recover + _rate_leave) * _state[t][STATE_INF];
		dstate[STATE_REC] = _rate_recover * _state[t][STATE_INF] - _rate_leave * _state[t][STATE_REC];

		for (int j(0); j < NUM_STATES; ++j)
			_state[t][j] += dstate[j] * _step_size;

		devent[EVENT_ENTER] += _step_size * _rate_enter;
		devent[EVENT_LEAVE] += _step_size * _rate_leave * popsize;
		devent[EVENT_TRANSMIT] += _step_size * force * _state[t][STATE_SUS];
		devent[EVENT_RECOVER] += _step_size * _rate_recover * _state[t][STATE_INF];
	}

	for (int j(0); j < NUM_EVENTS; ++j)
		_event[t][j] = devent[j];
}
