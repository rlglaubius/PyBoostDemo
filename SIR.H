#pragma once

#include <boost/multi_array.hpp>

class Array2D {
public:
	Array2D(const size_t ni, const size_t nj) : _values(boost::extents[ni][nj]) {}
	~Array2D() {}

	// Accessors
	inline double get(const size_t i, const size_t j) { return _values[i][j]; }
	inline void set(const size_t i, const size_t j, const double v) { _values[i][j] = v; }

	// Operator-based accessors
	inline double operator()(const size_t i, const size_t j) const { return _values[i][j]; } // enables double y; Array2D X(n,m); y = X(i,j);
	inline double& operator()(const size_t i, const size_t j) { return _values[i][j]; }      // enables double y; Array2D X(n,m); X(i,j) = y;

private:
	boost::multi_array<double, 2> _values;
};

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
	SIR(const size_t first_time, const size_t final_time);
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
	inline double state_count(const size_t t, const state_t x) const { return _state->get(t - _first_time, x); }
	inline double event_count(const size_t t, const event_t e) const { return _event->get(t - _first_time, e); }

private:
	int _final_time;
	int _first_time;
	int _num_units;    // number of unit time intervals between _first_time and _final_time (inclusive)
	int _num_steps;    // number of time steps per unit time
	double _step_size; // time step duration as a fraction of unit time

	double _rate_enter;
	double _rate_leave;
	double _rate_transmit;
	double _rate_recover;

	Array2D* _state;
	Array2D* _event;
};
