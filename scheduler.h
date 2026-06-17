class Scheduler: IETThread {
    // ...

    void run() {
        if (true) {
            /* R -> list of processes inside RQ 
            Time quantum slice, T */

            // SJF pre-emptive
            /* update R if there's any pending process to be scheduled
            If applicable, sort R where the first element satisfies P_0 = min(C)
            Select P_candidate, such that P_candidate = min(C)
            Execute P_candidate, Pcandidate.C-- 
            
            // Eound Eobin
            Update R if there's any pending process to be scheduled
            Select the first process in R (or randomyl) to be the P_candidate
            Execute P_candidate, P_candidate.C++. 
            If P_candidate.C = T, then perform #c. Put P_candidate at the end of R. Otherwise, perform #c
            */


        }
    }
}