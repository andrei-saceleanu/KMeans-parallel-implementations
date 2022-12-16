# KMeans++

## Descriere proiect

Pentru un set de n puncte/observatii unde fiecare punct este reprezentat
ca un vector d dimensional, k-means urmareste sa obtina o partitie 
de k multimi(clusters) a setului de puncte(fiecare set al partitiei avand un
element reprezentativ numit centroid).

Aceasta partitie rezultat ar trebui sa fie cea care minimizeaza
suma variantelor pentru fiecare set/cluster.

Fata de algoritmul clasic (vanilla k-means), varianta aleasa foloseste
o metoda diferita de initializare, ce va putea fi paralelizata.

Datasetul de input va fi unul sintetic, cuprinzand un numar mare de
vectori d-dimensionali generat printr-un script auxiliar Python.
(sklearn.datasets.make_blobs)

Proiectul isi propune paralelizarea diferitilor pasi ai algoritmului k-means++,
cu scopul de a observa performanta comparativa a mai multor metode de
paralelizare.


## Echipa:

Labau-Cristea Andrei-Liviu 343C1

Mandruta Cezar-Dan 343C1

Saceleanu Andrei-Iulian 343C1

## Asistent:

Ouatu Andrei-Catalin


## Detalii rulare

Din radacina proiectului, se poate realiza:

- Compilare pentru toate variantele: `make`

- Compilare pentru fiecare varianta in parte: `make app_kmeans`

    - app e un string in multimea {serial, openmp, mpi, pthreads}

- Rulare pentru fiecare varianta in parte: `make run_app` 

    - app e un string in multimea {serial, openmp, mpi, pthreads}
    - optiuni de rulare cu make:

        - TEST_NO=<b>N</b> - N este indicele fisierului de puncte de intrare (e.g. input_data/pointsN.txt), N apartine {1,2,3,4}
        - NP=<b>NP</b> - numarul de procesoare/thread-uri de utilizat(nu e necesar pentru versiunea seriala)
        - K=<b>K</b> - numarul de clustere/centroizi de identificat

- Stergere: `make clean`

## Implementare

**Limbaj de programare** : C

**Input**: un fisier de intrare si un numar de centroizi de identificat k

Format fisier de intrare:

- Prima linie - numarul de puncte n si dimensiunea unui punct d (2 valori intregi)

- Urmatoarele n linii - cate d valori float pentru fiecare punct

- Linie de separare - =====

- Ultimele linii pana la finalul fisierului - vectorii asociati centroizilor

**Output**: k centroizi / k linii rezultat cu cate d valori float


## Descrierea algoritmului

1. Se citesc argumentele programului
2. Se citesc din fisierul de intrare n, d (descrisi mai sus) si punctele de clusterizat
3. Se initializeaza prima generatie de centroizi folosind strategia k-means++

    - primul centroid se alege complet uniform aleator
    - la fiecare pas ulterior, fiecare punct are o probabilitate proportionala cu d<sup>2</sup> de a fi ales ca centroid initial
    - d - distanta minima a unui punct fata de oricare dintre centroizii deja alesi
    - initializarea se incheie cand exista k centroizi initiali

4. Pentru fiecare punct, se identifica centroidul cel mai apropiat/cluster-ul de care apartine
5. Actualizarea centroizilor

    - centroidul de index i va fi media punctelor asignate clusterului i
    - daca numarul de puncte asignate a fost 0, se alege un punct aleator din cele n disponibile.
6. Se repeta pasii 4. si 5. cat timp diferenta dintre 2 iteratii consecutive de centroizi este mai mare decat un threshold predefinit