# Weather Data Visualization Script
# Run with: gnuplot weather_plot.gnu

set terminal png size 1200,800 font "Arial,12"
set output 'weather_charts.png'

set multiplot layout 2,2 title "Los Angeles Weather Dashboard" font ",16"

# Chart 1: Temperature Bar Chart
set style fill solid 0.7
set boxwidth 0.7
set xtics rotate by -45
set ylabel "Temperature (°C)"
set title "Temperature Readings"
set grid ytics
set yrange [15:30]

plot '-' using 2:xtic(1) with boxes linecolor rgb "#FF6B6B" notitle
"Min" 20.0
"Current" 24.71
"Feels Like" 24.15
"Max" 26.44
e

# Chart 2: Humidity and Cloud Cover
set title "Humidity & Cloud Cover"
set ylabel "Percentage (%)"
set yrange [0:100]
set style data histograms
set style fill solid 0.7

plot '-' using 2:xtic(1) with boxes linecolor rgb "#4ECDC4" notitle
"Humidity" 35
"Clouds" 0
e

# Chart 3: Wind Speed Gauge
set title "Wind Speed"
set ylabel "Speed (m/s)"
set yrange [0:10]
unset xtics
set arrow from 0.5,0 to 0.5,5.66 head filled linecolor rgb "#95E1D3"

plot '-' using 1:2 with impulses linewidth 10 linecolor rgb "#95E1D3" notitle
0.5 5.66
e

# Chart 4: Pressure Comparison
set title "Pressure Levels"
set ylabel "Pressure (hPa)"
set yrange [990:1020]
set xtics rotate by 0

plot '-' using 2:xtic(1) with boxes linecolor rgb "#F38181" notitle
"Pressure" 1013
"Sea Level" 1013
"Ground" 994
e

unset multiplot