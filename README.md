Simple example of distributed computing program using C and PVM (Parallel Virtual Machine) tool.

Resolves problem of 'Roman Legioninsts' providing consistent movement of legions given:
- there is L legions still moved from one place to another
- given i-th legion consists of R[i] legnionists
- there is T tracts (T < L)
- every tract can contain in one moment t only t[j] legionists, doesn't matter which direction they're going to and:
min(t[j]) > max(R[j]) and sum(t[j]) < sum(R[j])
- after successfull movement legionists wait for order of moving to next localization
- tracts are the resources