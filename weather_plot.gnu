# Weather Data Visualization for Los Angeles
set terminal png size 1400,1000 font 'Arial,12'
set output 'weather_charts.png'

set multiplot layout 2,2 title 'Los Angeles Weather Data' font ',16'

# Temperature Chart
set style fill solid 0.7
set boxwidth 0.7
set ylabel 'Temperature (°C)'
set title 'Temperature Readings'
set grid ytics
set yrange [14:30]
plot '-' using 2:xtic(1) with boxes linecolor rgb '#FF6B6B' notitle
"Min" 19.58
"Current" 23.63
"Feels" 23.15
"Max" 25.81
e

# Humidity & Clouds
set title 'Humidity & Cloud Cover'
set ylabel 'Percentage (%  )'
set yrange [0:100]
plot '-' using 2:xtic(1) with boxes linecolor rgb '#4ECDC4' notitle
"Humidity" 42
"Clouds" 0
e

# Wind Speed
set title 'Wind Speed'
set ylabel 'Speed (m/s)'
set yrange [0:10]
plot '-' using 1:2 with impulses linewidth 15 linecolor rgb '#95E1D3' notitle
0.5 4.12
e

# Pressure
set title 'Pressure Levels'
set ylabel 'Pressure (hPa)'
set yrange [983:1023]
plot '-' using 2:xtic(1) with boxes linecolor rgb '#F38181' notitle
"Surface" 1013
"Sea Lvl" 1013
"Ground" 993
e

unset multiplot
