import numpy as np
import os
import sys
import x64.Debug.PyBoostDemo as SIR

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

    ## Create numpy arrays to store the results of model calculation
    work_state = np.empty([final_time - first_time + 1, SIR.num_states()], np.float64)
    work_event = np.empty([final_time - first_time + 1, SIR.num_events()], np.float64)

    model = SIR.SIR(first_time, final_time, work_state, work_event)
    model.initialize(rate_enter, rate_leave, rate_transmit, rate_recover, X0, Y0, Z0)
    model.calculate_proj()
    
    sys.stdout.write("Year,Susceptible,Infected,Recovered,Entries,Exits,Infections,Recoveries\n")
    for t in range(first_time, final_time + 1):
        sys.stdout.write("%d,%f,%f,%f,%f,%f,%f,%f\n" % (t,
                            work_state[t - first_time, 0],
                            work_state[t - first_time, 1],
                            work_state[t - first_time, 2],
                            work_event[t - first_time, 0],
                            work_event[t - first_time, 1],
                            work_event[t - first_time, 2],
                            work_event[t - first_time, 3]))

if __name__ == "__main__":    
    sys.stderr.write("Process %d" % (os.getpid()))
    main()

