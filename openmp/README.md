# Implementare OpenMP


## Version 1

- calculul celui mai apropiat centroid (i.e. functia compute_min_idx) cuprinde o bucla for ce itereaza prin toate punctele

    - paralelizare prin `omp parallel for`

- actualizarea centroizilor (i.e. functia update_centroids) este paralelizata astfel:

    - pentru fiecare centroid, thread-urile de OpenMP sunt in sync
    - pe portiunea de puncte alocata lor, fiecare calculeaza un count local si o suma locala a punctelor asignate centroidului curent analizat.
    - thread-ul principal reuneste rezultatele partiale


### Rezultate 

- testare preliminara pe partitia haswell (fep.grid.pub.ro) folosind fisierul de input points1.txt

- rularea variantei paralelizate OpenMP version 1 nu a determinat timpi de executie foarte buni

    - timpii au scazut insa eficienta este de maxim <b>15%</b>


<table>
<th>Numar thread-uri</th>
<th>Timp de rulare(s)</th>
<tr><td>1</td><td>16.093</td></tr>
<tr><td>2</td><td>16.522</td></tr>
<tr><td>4</td><td>11.604</td></tr>
<tr><td>8</td><td>10.3</td></tr>
<tr><td>16</td><td>7.338</td></tr>
<tr><td>32</td><td>6</td></tr>
</table>


### Concluzie

Trebuie aduse imbunatatiri la paralelizarea cu OpenMP pentru a creste eficienta computationala.


## Version 2


- utilizarea variabilelor pentru local count si local centroids este eficientizata cu ajutorul OpenMP

    - reduction prin suma aplicat pe C array (disponibil de la versiunea OpenMP 4.5.0)


### Rezultate 

- testare preliminara pe partitia haswell (fep.grid.pub.ro) folosind fisierul de input points1.txt

- rularea variantei paralelizate OpenMP version 2 a determinat accelerari mai bune decat version 1

    - eficienta de minim <b>30%</b>(in cel mai mare caz - P=32)

<table>
<th>Numar thread-uri</th>
<th>Timp de rulare(s)</th>
<tr><td>1</td><td>16.116</td></tr>
<tr><td>2</td><td>8.282</td></tr>
<tr><td>4</td><td>4.368</td></tr>
<tr><td>8</td><td>2.667</td></tr>
<tr><td>16</td><td>2.156</td></tr>
<tr><td>32</td><td>1.423</td></tr>
</table>