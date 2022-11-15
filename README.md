# KMeans++

## Descriere:

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


## Echipa:

Labau-Cristea Andrei-Liviu 343C1

Mandruta Cezar-Dan 343C1

Saceleanu Andrei-Iulian 343C1

## Asistent:

Ouatu Andrei-Catalin