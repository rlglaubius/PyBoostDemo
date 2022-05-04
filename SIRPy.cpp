#include "pch.h"
#include "SIR.h"

// This is the silo we trap all the Python-C++ interoperation incantations in

#define BOOST_PYTHON_STATIC_LIB
#define BOOST_NUMPY_STATIC_LIB

// Do not flip the order of these includes. It will cause compiler to
// try to use a dynamic library for boost.numpy instead of the static
// library, which will almost certainly be a showstopping runtime error
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

namespace py = boost::python;
namespace np = boost::python::numpy;

// Knows about python and numpy so SIR does not have to
class SIR_PyInterface {
public:
	SIR_PyInterface(const size_t first_time, const size_t final_time, np::ndarray& work_state, np::ndarray& work_event) {
		// Get pointers to the underlying raw data. ndarray::get_data() returns a char*, so
		// we need to cast it to double* (and hope that the user declared it accordingly)
		// TODO: do some error checking on work_state and work_event flags to make sure 
		// the data type and memory arrangement are consistent with what the SIR model expects.
		double* state_ptr = reinterpret_cast<double*>(work_state.get_data());
		double* event_ptr = reinterpret_cast<double*>(work_event.get_data());
		sir = new SIR(first_time, final_time, state_ptr, event_ptr);
	}

	~SIR_PyInterface() { if (sir != NULL) delete sir; }

	inline void initialize(
		const double rate_enter,
		const double rate_leave,
		const double rate_transmit,
		const double rate_recover,
		const double num_susceptible,
		const double num_infected,
		const double num_recovered) {
		sir->initialize(rate_enter, rate_leave, rate_transmit, rate_recover, num_susceptible, num_infected, num_recovered);
	}

	inline void calculate_proj() { sir->calculate(); }
	inline void calculate_unit(const size_t t) { sir->calculate(t); };
	inline double state_count(const size_t t, const SIR::state_t x) const { return sir->state_count(t, x); }
	inline double event_count(const size_t t, const SIR::event_t e) const { return sir->event_count(t, e); }

	// accessors that the python client can use to determine how much memory to allocate
	inline static size_t num_states() { return SIR::NUM_STATES; }
	inline static size_t num_events() { return SIR::NUM_EVENTS; }

private:
	SIR* sir;
};

// We rename enums, so that (e.g.) Python State.SUSCEPTIBLE correponds to C++ SIR::STATE_SUS. If we kept
// C++ names as-is, Python code would include painful redundandancies (e.g., Event.EVENT_ENTER)
BOOST_PYTHON_MODULE(PyBoostDemo) {
	boost::python::numpy::initialize(); // initialize numpy module

	boost::python::class_<SIR_PyInterface>("SIR", boost::python::init<size_t, size_t, np::ndarray&, np::ndarray&>())
		.def("initialize", &SIR_PyInterface::initialize)
		.def("calculate_proj", &SIR_PyInterface::calculate_proj)
		.def("calculate_unit", &SIR_PyInterface::calculate_unit)
		.def("state_count", &SIR_PyInterface::state_count)
		.def("event_count", &SIR_PyInterface::event_count)
	;

	boost::python::def("num_states", SIR_PyInterface::num_states);
	boost::python::def("num_events", SIR_PyInterface::num_events);

	boost::python::enum_<SIR::state_t>("State")
		.value("SUSCEPTIBLE", SIR::STATE_SUS)
		.value("INFECTED",    SIR::STATE_INF)
		.value("RECOVERED",   SIR::STATE_REC)
	;

	boost::python::enum_<SIR::event_t>("Event")
		.value("ENTER",    SIR::EVENT_ENTER)
		.value("LEAVE",    SIR::EVENT_LEAVE)
		.value("TRANSMIT", SIR::EVENT_TRANSMIT)
		.value("RECOVER",  SIR::EVENT_RECOVER)
	;
}

