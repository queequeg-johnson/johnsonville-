#include <stdio.h>
#include <time.h>

void print_weather_ascii() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    WEATHER REPORT                          ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║                                                            ║\n");
    printf("║                         \\   /                              ║\n");
    printf("║                          .-.                               ║\n");
    printf("║                       ― (   ) ―         ☀️                 ║\n");
    printf("║                          `-'                               ║\n");
    printf("║                         /   \\                              ║\n");
    printf("║                                                            ║\n");
    printf("║              📍 Los Angeles, US                            ║\n");
    printf("║              🌤️  Clear Sky                                 ║\n");
    printf("║                                                            ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  🌡️  Temperature                                           ║\n");
    printf("║     Current:    24.7°C  ████████████░░░░░░░░               ║\n");
    printf("║     Feels Like: 24.2°C  ███████████░░░░░░░░░               ║\n");
    printf("║     Min/Max:    20.0°C - 26.4°C                            ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  💧 Humidity:        35%%  ███████░░░░░░░░░░░░░░░           ║\n");
    printf("║  💨 Wind:           5.7 m/s  →→→  (W)                      ║\n");
    printf("║  🎚️  Pressure:      1013 hPa                               ║\n");
    printf("║  👁️  Visibility:    10.0 km                                ║\n");
    printf("║  ☁️  Clouds:         0%%  ░░░░░░░░░░░░░░░░░░░░             ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  📊 Pressure Levels:                                       ║\n");
    printf("║     Sea Level:    1013 hPa ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓               ║\n");
    printf("║     Ground Level:  994 hPa ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓                ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  🗺️  Coordinates: 34.05°N, 118.24°W                        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Weather condition emoji art
    printf("        Current Conditions\n");
    printf("        ═══════════════════\n");
    printf("              ☀️ ☀️ ☀️\n");
    printf("           Clear & Sunny!\n");
    printf("        Temperature: 75.7°F\n");
    printf("         Perfect weather! 🌈\n");
    printf("\n");
}

int main() {
    print_weather_ascii();
    return 0;
}