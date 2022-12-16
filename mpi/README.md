# Implementare MPI


## Version 1

- calculul celui mai apropiat centroid (i.e. functia compute_min_idx) cuprinde o bucla for ce itereaza prin toate punctele
- acestea au fost impartite astfel incat fiecare proces sa se ocupe de un numar egal de puncte
- procesul 0 (MASTER) are rolul de agregator, acesta primind fractiunile din vectorul min_idx de la toate celelalte procese
- codul functiei a fost adaugat direct in functia main pentru usurinta paralelizarii

- actualizarea centroizilor (i.e. functia update_centroids) este paralelizata astfel:

    - pentru fiecare proces exista un vector local de centroizi
    - pe portiunea de puncte alocata lor, fiecare proces calculeaza un count local si o suma locala a punctelor asignate centroidului curent analizat.
    - procesul MASTER reuneste rezultatele partiale si realizeaza restul operatiilor necesare si afisarea


### Rezultate 

- testare preliminara pe masina locala (max. 12 CPUs) folosind setul de input points1.txt de dimensiune 500000 puncte

- rularea variantei paralelizate MPI pentru diferite valori k (numar de clustere) a dat urmatoarele rezulate


**K = 2**
<table>
<th>Numar procese</th>
<th>Timp de rulare(s)</th>
<tr><td>1</td><td>0.901</td></tr>
<tr><td>2</td><td>0.799</td></tr>
<tr><td>4</td><td>0.919</td></tr>
<tr><td>6</td><td>1.122</td></tr>
<tr><td>8</td><td>1.15</td></tr>
<tr><td>10</td><td>1.009</td></tr>
<tr><td>12</td><td>1.46</td></tr>
</table>


**K = 4**
<table>
<th>Numar procese</th>
<th>Timp de rulare(s)</th>
<tr><td>1</td><td>1.026</td></tr>
<tr><td>2</td><td>0.976</td></tr>
<tr><td>4</td><td>1.097</td></tr>
<tr><td>6</td><td>1.141</td></tr>
<tr><td>8</td><td>1.004</td></tr>
<tr><td>10</td><td>1.25</td></tr>
<tr><td>12</td><td>1.732</td></tr>
</table>


**K = 5**
<table>
<th>Numar procese</th>
<th>Timp de rulare(s)</th>
<tr><td>1</td><td>10.966</td></tr>
<tr><td>2</td><td>11.926</td></tr>
<tr><td>4</td><td>5.782</td></tr>
<tr><td>6</td><td>4.819</td></tr>
<tr><td>8</td><td>4.401</td></tr>
<tr><td>10</td><td>3.195</td></tr>
<tr><td>12</td><td>2.034</td></tr>
</table>

**K = 8**
<table>
<th>Numar procese</th>
<th>Timp de rulare(s)</th>
<tr><td>1</td><td>21.412</td></tr>
<tr><td>2</td><td>23.158</td></tr>
<tr><td>4</td><td>7.811</td></tr>
<tr><td>6</td><td>7.782</td></tr>
<tr><td>8</td><td>5.859</td></tr>
<tr><td>10</td><td>6.473</td></tr>
<tr><td>12</td><td>5.911</td></tr>
</table>

**K = 20**
<table>
<th>Numar procese</th>
<th>Timp de rulare(s)</th>
<tr><td>1</td><td>116.01</td></tr>
<tr><td>2</td><td>77.654</td></tr>
<tr><td>4</td><td>61.024</td></tr>
<tr><td>6</td><td>47.046</td></tr>
<tr><td>8</td><td>37.65</td></tr>
<tr><td>10</td><td>32.17</td></tr>
<tr><td>12</td><td>30.219</td></tr>
</table>


### Concluzie

Implementarea paralela MPI s-a dovedit a fi eficienta in cazul in care vrem sa calculam un numar de centroizi >= 5. Pentru un numar mai mic de clustere timpii sunt mult prea mici pentru a se observa o imbunatatire semnificativa. Din potriva, in unele cazuri, paralelizarea scade performantele temporale ale algoritmului.
