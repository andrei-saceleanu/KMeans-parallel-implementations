set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2
unset key
set title "Title"
set ylabel "Y Title"
set xlabel "X Title"
set xrange [0:32]
set yrange [0:17]
plot ARG1 using 1:2:xtic(1):ytic(2) with lines linestyle 1
pause -1 "Hit any key to continue"
