import os
import sys
from x64.Debug.PyBoostDemo import SIR, State, Event

## x64.Debug.PyBoostDemo tells python to look for the PYD in .\x64\Debug. For optimized versions
## without debugging symbols, we should look in x64.Release.PyBoostDemo instead

def main():
    initsize = 1000000.0
    initprev = 0.001
    rate_leave = 1.0 / 35.0
    rate_enter = initsize * rate_leave
    rate_recover = 0.1
    rate_transmit = 7.0 * rate_recover
    first_time = 1970
    final_time = 2030

    X0 = initsize * (1.0 - initprev)
    Y0 = initsize - X0
    Z0 = 0.0

    model = SIR(first_time, final_time)
    model.initialize(rate_enter, rate_leave, rate_transmit, rate_recover, X0, Y0, Z0)
    model.calculate_proj()
    
    sys.stdout.write("Year,Susceptible,Infected,Recovered,Entries,Exits,Infections,Recoveries\n")
    for t in range(first_time, final_time + 1):
        sys.stdout.write("%d,%f,%f,%f,%f,%f,%f,%f\n" % (t,
                                                      model.state_count(t,State.SUSCEPTIBLE),
                                                      model.state_count(t,State.INFECTED),
                                                      model.state_count(t,State.RECOVERED),
                                                      model.event_count(t,Event.ENTER),
                                                      model.event_count(t,Event.LEAVE),
                                                      model.event_count(t,Event.TRANSMIT),
                                                      model.event_count(t,Event.RECOVER)))

if __name__ == "__main__":    
    sys.stderr.write("Process %d" % (os.getpid()))
    main()

