#include "pch.h"
#include "SIR.h"

#define BOOST_PYTHON_STATIC_LIB

#include <boost/python.hpp>

// Since python does not support function overloading, we rename overloaded SIR methods (e.g., calculate() and
// calculate(t) are renamed calculate_proj() and calculate_unit(t)) to disambiguate. Unfortunately, the syntax
// for doing this disambiguation is arcane (e.g, "void (SIR::*)(int)" is a pointer to a member function of the
// SIR class; that function takes an int as an argument and returns nothing).
//
// We also rename enums, so that (e.g.) Python State.SUSCEPTIBLE correponds to C++ SIR::STATE_SUS. If we kept
// C++ names as-is, Python code would include painful redundandancies (e.g., Event.EVENT_ENTER)
BOOST_PYTHON_MODULE(PyBoostDemo) {
	boost::python::class_<SIR>("SIR", boost::python::init<size_t,size_t>())
		.def("initialize", &SIR::initialize)
		.def<void (SIR::*)(void)>("calculate_proj", &SIR::calculate)
		.def<void (SIR::*)(size_t)>("calculate_unit", &SIR::calculate)
		.def("state_count", &SIR::state_count)
		.def("event_count", &SIR::event_count)
	;

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

