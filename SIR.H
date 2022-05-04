#ifndef SIR_H
#define SIR_H

#include <boost/multi_array.hpp>

class SIR {
public:
	enum state_t {
		STATE_SUS = 0,
		STATE_INF = 1,
		STATE_REC = 2,
		NUM_STATES = 3
	};

	enum event_t {
		EVENT_ENTER = 0,
		EVENT_LEAVE = 1,
		EVENT_TRANSMIT = 2,
		EVENT_RECOVER = 3,
		NUM_EVENTS = 4
	};

	// The model will store outcomes for discrete (unit) times first_time >= t >= final_time.
	// The user is responsible for specifying rate per unit time
	//
	// The client application is responsible for managing storage used by SIR instances
	// - work_state is used to store compartment sizes. Size must be at least (final_time - first_time + 1) * 3
	// - work_event is used to store event counts. Size must be at least (final_time - first_time + 1) * 4
	// WARNING: memory access violations are likely if these are not sized correctly
	SIR(const size_t first_time, const size_t final_time, double* const work_state, double* const work_event);
	~SIR();

	// convenience function: set all the inputs at once
	void initialize(
		const double rate_enter,
		const double rate_leave,
		const double rate_transmit,
		const double rate_recover,
		const double num_susceptible,
		const double num_infected,
		const double num_recovered);

	void calculate();
	void calculate(size_t t);

	// Getters
	double rateEnter() const { return _rate_enter; }
	double rateLeave() const { return _rate_leave; }
	double rateTransmit() const { return _rate_transmit; }
	double rateRecover() const { return _rate_recover; }

	// Setters
	void rateEnter(const double rate_enter) { _rate_enter = rate_enter; }
	void rateLeave(const double rate_leave) { _rate_leave = rate_leave; }
	void rateTransmit(const double rate_transmit) { _rate_transmit = rate_transmit; }
	void rateRecover(const double rate_recover) { _rate_recover = rate_recover; }

	// Check population sizes and event counts
	inline double state_count(const size_t t, const state_t x) const { return _state[t - _first_time][x]; }
	inline double event_count(const size_t t, const event_t e) const { return _event[t - _first_time][e]; }

private:
	size_t _final_time;
	size_t _first_time;
	size_t _num_units; // number of unit time intervals between _first_time and _final_time (inclusive)
	int _num_steps;    // number of time steps per unit time
	double _step_size; // time step duration as a fraction of unit time

	double _rate_enter;
	double _rate_leave;
	double _rate_transmit;
	double _rate_recover;

	boost::multi_array_ref<double, 2> _state;
	boost::multi_array_ref<double, 2> _event;
};

#endif // SIR_H