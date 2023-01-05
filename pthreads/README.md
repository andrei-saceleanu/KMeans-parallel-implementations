# Implementare Pthreads

## Version 1

Folosind biblioteca pthreads.h, am pastrat aceeasi logica de paralelizare ca si in implementarea cu OpenMP, si anume am paralelizat:

- calculul celui mai apropiat centroid (i.e functia compute_min_idx)
- actualizarea centroizilor (i.e. functia update_centroids)

Pentru a reduce cat mai mult overhead-ul adus de operatiile ce asigura management-ul thread-urilor, am ales sa structurez codul in asa fel incat thread-urile sa fie pornite o singura data in program.

Pentru sincronizarea thread-urilor intre diferitele sectiuni ale codului / diferitele iteratii ale buclelor for, am folosit o bariera: **_pthread_barrier_t\* bar_**.

A fost necesara si utilizarea unui mutex, pentru a evita race condition-urile care apareau la scrierea concurenta in aceeasi variabila de catre mai multe thread-uri: **_pthread_mutex_t\* lock_**.

Rulari diferite ale variantei cu pthreads pot genera rezultate usor diferite, cu diferente de ordinul sutimilor, intrucat pot aparea erori de rotunjire la operatiile cu floating point numbers.

### Rezultate

Rezultatele si concluziile rularii pe grid a variantei cu pthreads a algoritmului poate fi gasita [aici](https://gitlab.cs.pub.ro/app-2022/kmeans-pp/-/wikis/Descrierea-implementarilor-paralele#profiling-1).
