#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <math.h>

#define API_KEY "b07024da23edbf67525c2bbe2786aafb"
#define CITY "Los%20Angeles"
#define COUNTRY "US"

// Structure to hold response data
typedef struct {
    char *data;
    size_t size;
} ResponseData;

// Weather data structure
typedef struct {
    char city[50];
    char country[10];
    char description[100];
    char main_weather[50];
    double temp;
    double feels_like;
    double temp_min;
    double temp_max;
    int humidity;
    int pressure;
    int sea_level;
    int grnd_level;
    double wind_speed;
    int wind_deg;
    int clouds;
    int visibility;
    double lon;
    double lat;
} WeatherData;

// Callback function to capture curl response
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    ResponseData *mem = (ResponseData *)userp;
    
    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if(!ptr) {
        printf("Not enough memory!\n");
        return 0;
    }
    
    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;
    
    return realsize;
}

// Parse JSON response
int parse_weather_json(const char *json_str, WeatherData *data) {
    struct json_object *parsed_json;
    struct json_object *coord, *weather, *main, *wind, *clouds, *sys;
    struct json_object *temp_obj;
    
    parsed_json = json_tokener_parse(json_str);
    if (!parsed_json) {
        printf("Error parsing JSON!\n");
        return 0;
    }
    
    // Parse coordinates
    json_object_object_get_ex(parsed_json, "coord", &coord);
    json_object_object_get_ex(coord, "lon", &temp_obj);
    data->lon = json_object_get_double(temp_obj);
    json_object_object_get_ex(coord, "lat", &temp_obj);
    data->lat = json_object_get_double(temp_obj);
    
    // Parse weather description
    json_object_object_get_ex(parsed_json, "weather", &weather);
    struct json_object *weather_item = json_object_array_get_idx(weather, 0);
    json_object_object_get_ex(weather_item, "description", &temp_obj);
    strncpy(data->description, json_object_get_string(temp_obj), sizeof(data->description) - 1);
    json_object_object_get_ex(weather_item, "main", &temp_obj);
    strncpy(data->main_weather, json_object_get_string(temp_obj), sizeof(data->main_weather) - 1);
    
    // Parse main weather data
    json_object_object_get_ex(parsed_json, "main", &main);
    json_object_object_get_ex(main, "temp", &temp_obj);
    data->temp = json_object_get_double(temp_obj);
    json_object_object_get_ex(main, "feels_like", &temp_obj);
    data->feels_like = json_object_get_double(temp_obj);
    json_object_object_get_ex(main, "temp_min", &temp_obj);
    data->temp_min = json_object_get_double(temp_obj);
    json_object_object_get_ex(main, "temp_max", &temp_obj);
    data->temp_max = json_object_get_double(temp_obj);
    json_object_object_get_ex(main, "pressure", &temp_obj);
    data->pressure = json_object_get_int(temp_obj);
    json_object_object_get_ex(main, "humidity", &temp_obj);
    data->humidity = json_object_get_int(temp_obj);
    
    // Optional: sea_level and grnd_level
    if (json_object_object_get_ex(main, "sea_level", &temp_obj))
        data->sea_level = json_object_get_int(temp_obj);
    else
        data->sea_level = data->pressure;
    
    if (json_object_object_get_ex(main, "grnd_level", &temp_obj))
        data->grnd_level = json_object_get_int(temp_obj);
    else
        data->grnd_level = data->pressure;
    
    // Parse wind
    json_object_object_get_ex(parsed_json, "wind", &wind);
    json_object_object_get_ex(wind, "speed", &temp_obj);
    data->wind_speed = json_object_get_double(temp_obj);
    if (json_object_object_get_ex(wind, "deg", &temp_obj))
        data->wind_deg = json_object_get_int(temp_obj);
    else
        data->wind_deg = 0;
    
    // Parse clouds
    json_object_object_get_ex(parsed_json, "clouds", &clouds);
    json_object_object_get_ex(clouds, "all", &temp_obj);
    data->clouds = json_object_get_int(temp_obj);
    
    // Parse visibility
    if (json_object_object_get_ex(parsed_json, "visibility", &temp_obj))
        data->visibility = json_object_get_int(temp_obj);
    else
        data->visibility = 10000;
    
    // Parse city name
    json_object_object_get_ex(parsed_json, "name", &temp_obj);
    strncpy(data->city, json_object_get_string(temp_obj), sizeof(data->city) - 1);
    
    // Parse country
    json_object_object_get_ex(parsed_json, "sys", &sys);
    json_object_object_get_ex(sys, "country", &temp_obj);
    strncpy(data->country, json_object_get_string(temp_obj), sizeof(data->country) - 1);
    
    json_object_put(parsed_json);
    return 1;
}

// Get weather emoji based on condition
const char* get_weather_emoji(const char* main_weather, int clouds) {
    if (strcmp(main_weather, "Clear") == 0) return "☀️";
    if (strcmp(main_weather, "Clouds") == 0) {
        if (clouds < 30) return "🌤️";
        if (clouds < 70) return "⛅";
        return "☁️";
    }
    if (strcmp(main_weather, "Rain") == 0) return "🌧️";
    if (strcmp(main_weather, "Drizzle") == 0) return "🌦️";
    if (strcmp(main_weather, "Thunderstorm") == 0) return "⛈️";
    if (strcmp(main_weather, "Snow") == 0) return "❄️";
    if (strcmp(main_weather, "Mist") == 0 || strcmp(main_weather, "Fog") == 0) return "🌫️";
    return "🌡️";
}

// Generate ASCII art display
void generate_ascii(WeatherData *data) {
    const char* emoji = get_weather_emoji(data->main_weather, data->clouds);
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    WEATHER REPORT                          ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    // Weather icon art
    if (strcmp(data->main_weather, "Clear") == 0) {
        printf("║                         \\   /                              ║\n");
        printf("║                          .-.                               ║\n");
        printf("║                       ― (   ) ―         ☀️                 ║\n");
        printf("║                          `-'                               ║\n");
        printf("║                         /   \\                              ║\n");
    } else if (strcmp(data->main_weather, "Clouds") == 0) {
        printf("║                         .--.                               ║\n");
        printf("║                      .-(    ).                             ║\n");
        printf("║                     (___.__)__)        ☁️                  ║\n");
        printf("║                                                            ║\n");
        printf("║                                                            ║\n");
    } else if (strcmp(data->main_weather, "Rain") == 0) {
        printf("║                         .--.                               ║\n");
        printf("║                      .-(    ).                             ║\n");
        printf("║                     (___.__)__)        🌧️                  ║\n");
        printf("║                      ʻ ʻ ʻ ʻ                               ║\n");
        printf("║                     ʻ ʻ ʻ ʻ                                ║\n");
    } else {
        printf("║                                                            ║\n");
        printf("║                         %s                                  ║\n", emoji);
        printf("║                                                            ║\n");
        printf("║                                                            ║\n");
        printf("║                                                            ║\n");
    }
    
    printf("║                                                            ║\n");
    printf("║              📍 %-20s, %-2s                       ║\n", data->city, data->country);
    printf("║              🌤️  %-30s            ║\n", data->description);
    printf("║                                                            ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  🌡️  Temperature:                                          ║\n");
    printf("║     Current:    %5.1f°C  ", data->temp);
    
    // Temperature bar
    int temp_bar = (int)((data->temp - 15) / 20.0 * 20);
    if (temp_bar < 0) temp_bar = 0;
    if (temp_bar > 20) temp_bar = 20;
    for(int i = 0; i < 20; i++) {
        printf(i < temp_bar ? "█" : "░");
    }
    printf("       ║\n");
    
    printf("║     Feels Like: %5.1f°C                                    ║\n", data->feels_like);
    printf("║     Min/Max:    %5.1f°C - %5.1f°C                         ║\n", 
           data->temp_min, data->temp_max);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  💧 Humidity:       %3d%%  ", data->humidity);
    
    // Humidity bar
    int hum_bar = data->humidity / 5;
    for(int i = 0; i < 20; i++) {
        printf(i < hum_bar ? "█" : "░");
    }
    printf("           ║\n");
    
    // Wind direction
    const char* wind_dir = "N";
    if (data->wind_deg >= 337.5 || data->wind_deg < 22.5) wind_dir = "N";
    else if (data->wind_deg < 67.5) wind_dir = "NE";
    else if (data->wind_deg < 112.5) wind_dir = "E";
    else if (data->wind_deg < 157.5) wind_dir = "SE";
    else if (data->wind_deg < 202.5) wind_dir = "S";
    else if (data->wind_deg < 247.5) wind_dir = "SW";
    else if (data->wind_deg < 292.5) wind_dir = "W";
    else wind_dir = "NW";
    
    printf("║  💨 Wind:          %4.1f m/s  →→→  (%s)                    ║\n", data->wind_speed, wind_dir);
    printf("║  🎚️  Pressure:     %4d hPa                                ║\n", data->pressure);
    printf("║  👁️  Visibility:   %4.1f km                                ║\n", data->visibility/1000.0);
    printf("║  ☁️  Clouds:        %3d%%  ", data->clouds);
    
    // Cloud bar
    int cloud_bar = data->clouds / 5;
    for(int i = 0; i < 20; i++) {
        printf(i < cloud_bar ? "█" : "░");
    }
    printf("             ║\n");
    
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  📊 Pressure Levels:                                       ║\n");
    printf("║     Sea Level:    %4d hPa ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓               ║\n", data->sea_level);
    printf("║     Ground Level: %4d hPa ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓                ║\n", data->grnd_level);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  🗺️  Coordinates: %.2f°%c, %.2f°%c                     ║\n", 
           fabs(data->lat), data->lat >= 0 ? 'N' : 'S',
           fabs(data->lon), data->lon >= 0 ? 'E' : 'W');
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

// Generate HTML file
void generate_html(WeatherData *data) {
    FILE *fp = fopen("weather_dashboard.html", "w");
    if (!fp) {
        printf("❌ Error creating HTML file!\n");
        return;
    }
    
    const char* emoji = get_weather_emoji(data->main_weather, data->clouds);
    
    fprintf(fp, "<!DOCTYPE html>\n");
    fprintf(fp, "<html lang=\"en\">\n<head>\n");
    fprintf(fp, "    <meta charset=\"UTF-8\">\n");
    fprintf(fp, "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(fp, "    <title>%s Weather Dashboard</title>\n", data->city);
    fprintf(fp, "    <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n");
    fprintf(fp, "    <style>\n");
    fprintf(fp, "        body {\n");
    fprintf(fp, "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n");
    fprintf(fp, "            background: linear-gradient(135deg, #667eea 0%%, #764ba2 100%%);\n");
    fprintf(fp, "            margin: 0;\n");
    fprintf(fp, "            padding: 20px;\n");
    fprintf(fp, "            min-height: 100vh;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .container {\n");
    fprintf(fp, "            max-width: 1200px;\n");
    fprintf(fp, "            margin: 0 auto;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .header {\n");
    fprintf(fp, "            text-align: center;\n");
    fprintf(fp, "            color: white;\n");
    fprintf(fp, "            margin-bottom: 30px;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .header h1 {\n");
    fprintf(fp, "            font-size: 3em;\n");
    fprintf(fp, "            margin: 10px 0;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .weather-icon {\n");
    fprintf(fp, "            font-size: 5em;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .cards {\n");
    fprintf(fp, "            display: grid;\n");
    fprintf(fp, "            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));\n");
    fprintf(fp, "            gap: 20px;\n");
    fprintf(fp, "            margin-bottom: 30px;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .card {\n");
    fprintf(fp, "            background: rgba(255, 255, 255, 0.95);\n");
    fprintf(fp, "            border-radius: 15px;\n");
    fprintf(fp, "            padding: 25px;\n");
    fprintf(fp, "            box-shadow: 0 8px 16px rgba(0, 0, 0, 0.2);\n");
    fprintf(fp, "            transition: transform 0.3s;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .card:hover {\n");
    fprintf(fp, "            transform: translateY(-5px);\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .card-title {\n");
    fprintf(fp, "            color: #666;\n");
    fprintf(fp, "            font-size: 0.9em;\n");
    fprintf(fp, "            text-transform: uppercase;\n");
    fprintf(fp, "            letter-spacing: 1px;\n");
    fprintf(fp, "            margin-bottom: 10px;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .card-value {\n");
    fprintf(fp, "            font-size: 2.5em;\n");
    fprintf(fp, "            font-weight: bold;\n");
    fprintf(fp, "            color: #333;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .card-unit {\n");
    fprintf(fp, "            font-size: 0.5em;\n");
    fprintf(fp, "            color: #999;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .card-icon {\n");
    fprintf(fp, "            font-size: 2em;\n");
    fprintf(fp, "            margin-bottom: 10px;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .chart-container {\n");
    fprintf(fp, "            background: rgba(255, 255, 255, 0.95);\n");
    fprintf(fp, "            border-radius: 15px;\n");
    fprintf(fp, "            padding: 25px;\n");
    fprintf(fp, "            box-shadow: 0 8px 16px rgba(0, 0, 0, 0.2);\n");
    fprintf(fp, "            margin-bottom: 20px;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .details {\n");
    fprintf(fp, "            background: rgba(255, 255, 255, 0.95);\n");
    fprintf(fp, "            border-radius: 15px;\n");
    fprintf(fp, "            padding: 25px;\n");
    fprintf(fp, "            box-shadow: 0 8px 16px rgba(0, 0, 0, 0.2);\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .detail-row {\n");
    fprintf(fp, "            display: flex;\n");
    fprintf(fp, "            justify-content: space-between;\n");
    fprintf(fp, "            padding: 10px 0;\n");
    fprintf(fp, "            border-bottom: 1px solid #eee;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .detail-row:last-child {\n");
    fprintf(fp, "            border-bottom: none;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "    </style>\n");
    fprintf(fp, "</head>\n<body>\n");
    fprintf(fp, "    <div class=\"container\">\n");
    fprintf(fp, "        <div class=\"header\">\n");
    fprintf(fp, "            <div class=\"weather-icon\">%s</div>\n", emoji);
    fprintf(fp, "            <h1>%s</h1>\n", data->city);
    fprintf(fp, "            <p style=\"font-size: 1.5em;\">%s</p>\n", data->description);
    fprintf(fp, "        </div>\n\n");
    
    fprintf(fp, "        <div class=\"cards\">\n");
    fprintf(fp, "            <div class=\"card\">\n");
    fprintf(fp, "                <div class=\"card-icon\">🌡️</div>\n");
    fprintf(fp, "                <div class=\"card-title\">Temperature</div>\n");
    fprintf(fp, "                <div class=\"card-value\">%.1f<span class=\"card-unit\">°C</span></div>\n", data->temp);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"card\">\n");
    fprintf(fp, "                <div class=\"card-icon\">🤚</div>\n");
    fprintf(fp, "                <div class=\"card-title\">Feels Like</div>\n");
    fprintf(fp, "                <div class=\"card-value\">%.1f<span class=\"card-unit\">°C</span></div>\n", data->feels_like);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"card\">\n");
    fprintf(fp, "                <div class=\"card-icon\">💧</div>\n");
    fprintf(fp, "                <div class=\"card-title\">Humidity</div>\n");
    fprintf(fp, "                <div class=\"card-value\">%d<span class=\"card-unit\">%%</span></div>\n", data->humidity);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"card\">\n");
    fprintf(fp, "                <div class=\"card-icon\">💨</div>\n");
    fprintf(fp, "                <div class=\"card-title\">Wind Speed</div>\n");
    fprintf(fp, "                <div class=\"card-value\">%.1f<span class=\"card-unit\">m/s</span></div>\n", data->wind_speed);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "        </div>\n\n");
    
    fprintf(fp, "        <div class=\"chart-container\">\n");
    fprintf(fp, "            <h2 style=\"margin-top: 0;\">Temperature Range</h2>\n");
    fprintf(fp, "            <canvas id=\"tempChart\"></canvas>\n");
    fprintf(fp, "        </div>\n\n");
    
    fprintf(fp, "        <div class=\"chart-container\">\n");
    fprintf(fp, "            <h2 style=\"margin-top: 0;\">Weather Metrics</h2>\n");
    fprintf(fp, "            <canvas id=\"metricsChart\"></canvas>\n");
    fprintf(fp, "        </div>\n\n");
    
    fprintf(fp, "        <div class=\"details\">\n");
    fprintf(fp, "            <h2 style=\"margin-top: 0;\">Detailed Information</h2>\n");
    fprintf(fp, "            <div class=\"detail-row\">\n");
    fprintf(fp, "                <span><strong>Minimum Temperature:</strong></span>\n");
    fprintf(fp, "                <span>%.1f°C</span>\n", data->temp_min);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"detail-row\">\n");
    fprintf(fp, "                <span><strong>Maximum Temperature:</strong></span>\n");
    fprintf(fp, "                <span>%.1f°C</span>\n", data->temp_max);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"detail-row\">\n");
    fprintf(fp, "                <span><strong>Pressure:</strong></span>\n");
    fprintf(fp, "                <span>%d hPa</span>\n", data->pressure);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"detail-row\">\n");
    fprintf(fp, "                <span><strong>Sea Level Pressure:</strong></span>\n");
    fprintf(fp, "                <span>%d hPa</span>\n", data->sea_level);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"detail-row\">\n");
    fprintf(fp, "                <span><strong>Ground Level Pressure:</strong></span>\n");
    fprintf(fp, "                <span>%d hPa</span>\n", data->grnd_level);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"detail-row\">\n");
    fprintf(fp, "                <span><strong>Visibility:</strong></span>\n");
    fprintf(fp, "                <span>%.1f km</span>\n", data->visibility/1000.0);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"detail-row\">\n");
    fprintf(fp, "                <span><strong>Wind Direction:</strong></span>\n");
    fprintf(fp, "                <span>%d°</span>\n", data->wind_deg);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"detail-row\">\n");
    fprintf(fp, "                <span><strong>Cloud Coverage:</strong></span>\n");
    fprintf(fp, "                <span>%d%%</span>\n", data->clouds);
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"detail-row\">\n");
    fprintf(fp, "                <span><strong>Coordinates:</strong></span>\n");
    fprintf(fp, "                <span>%.2f°%c, %.2f°%c</span>\n", 
            fabs(data->lat), data->lat >= 0 ? 'N' : 'S',
            fabs(data->lon), data->lon >= 0 ? 'E' : 'W');
    fprintf(fp, "            </div>\n");
    fprintf(fp, "        </div>\n");
    fprintf(fp, "    </div>\n\n");
    
    fprintf(fp, "    <script>\n");
    fprintf(fp, "        const tempCtx = document.getElementById('tempChart').getContext('2d');\n");
    fprintf(fp, "        new Chart(tempCtx, {\n");
    fprintf(fp, "            type: 'bar',\n");
    fprintf(fp, "            data: {\n");
    fprintf(fp, "                labels: ['Minimum', 'Current', 'Feels Like', 'Maximum'],\n");
    fprintf(fp, "                datasets: [{\n");
    fprintf(fp, "                    label: 'Temperature (°C)',\n");
    fprintf(fp, "                    data: [%.2f, %.2f, %.2f, %.2f],\n", 
            data->temp_min, data->temp, data->feels_like, data->temp_max);
    fprintf(fp, "                    backgroundColor: [\n");
    fprintf(fp, "                        'rgba(54, 162, 235, 0.7)',\n");
    fprintf(fp, "                        'rgba(255, 206, 86, 0.7)',\n");
    fprintf(fp, "                        'rgba(255, 159, 64, 0.7)',\n");
    fprintf(fp, "                        'rgba(255, 99, 132, 0.7)'\n");
    fprintf(fp, "                    ],\n");
    fprintf(fp, "                    borderColor: [\n");
    fprintf(fp, "                        'rgba(54, 162, 235, 1)',\n");
    fprintf(fp, "                        'rgba(255, 206, 86, 1)',\n");
    fprintf(fp, "                        'rgba(255, 159, 64, 1)',\n");
    fprintf(fp, "                        'rgba(255, 99, 132, 1)'\n");
    fprintf(fp, "                    ],\n");
    fprintf(fp, "                    borderWidth: 2\n");
    fprintf(fp, "                }]\n");
    fprintf(fp, "            },\n");
    fprintf(fp, "            options: {\n");
    fprintf(fp, "                responsive: true,\n");
    fprintf(fp, "                scales: {\n");
    fprintf(fp, "                    y: {\n");
    fprintf(fp, "                        beginAtZero: false\n");
    fprintf(fp, "                    }\n");
    fprintf(fp, "                },\n");
    fprintf(fp, "                plugins: {\n");
    fprintf(fp, "                    legend: {\n");
    fprintf(fp, "                        display: false\n");
    fprintf(fp, "                    }\n");
    fprintf(fp, "                }\n");
    fprintf(fp, "            }\n");
    fprintf(fp, "        });\n\n");
    
    fprintf(fp, "        const metricsCtx = document.getElementById('metricsChart').getContext('2d');\n");
    fprintf(fp, "        new Chart(metricsCtx, {\n");
    fprintf(fp, "            type: 'doughnut',\n");
    fprintf(fp, "            data: {\n");
    fprintf(fp, "                labels: ['Humidity', 'Cloud Cover', 'Clear Sky'],\n");
    fprintf(fp, "                datasets: [{\n");
    fprintf(fp, "                    data: [%d, %d, %d],\n", data->humidity, data->clouds, 100 - data->clouds);
    fprintf(fp, "                    backgroundColor: [\n");
    fprintf(fp, "                        'rgba(54, 162, 235, 0.7)',\n");
    fprintf(fp, "                        'rgba(201, 203, 207, 0.7)',\n");
    fprintf(fp, "                        'rgba(255, 205, 86, 0.7)'\n");
    fprintf(fp, "                    ],\n");
    fprintf(fp, "                    borderWidth: 2\n");
    fprintf(fp, "                }]\n");
    fprintf(fp, "            },\n");
    fprintf(fp, "            options: {\n");
    fprintf(fp, "                responsive: true,\n");
    fprintf(fp, "                plugins: {\n");
    fprintf(fp, "                    legend: {\n");
    fprintf(fp, "                        position: 'bottom'\n");
    fprintf(fp, "                    }\n");
    fprintf(fp, "                }\n");
    fprintf(fp, "            }\n");
    fprintf(fp, "        });\n");
    fprintf(fp, "    </script>\n");
    fprintf(fp, "</body>\n</html>\n");
    
    fclose(fp);
    printf("✅ HTML dashboard created: weather_dashboard.html\n");
}

// Generate gnuplot script
void generate_gnuplot(WeatherData *data) {
    FILE *fp = fopen("weather_plot.gnu", "w");
    if (!fp) {
        printf("❌ Error creating gnuplot file!\n");
        return;
    }
    
    fprintf(fp, "# Weather Data Visualization for %s\n", data->city);
    fprintf(fp, "set terminal png size 1400,1000 font 'Arial,12'\n");
    fprintf(fp, "set output 'weather_charts.png'\n\n");
    
    fprintf(fp, "set multiplot layout 2,2 title '%s Weather Data' font ',16'\n\n", data->city);
    
    // Temperature chart
    fprintf(fp, "# Temperature Chart\n");
    fprintf(fp, "set style fill solid 0.7\n");
    fprintf(fp, "set boxwidth 0.7\n");
    fprintf(fp, "set ylabel 'Temperature (°C)'\n");
    fprintf(fp, "set title 'Temperature Readings'\n");
    fprintf(fp, "set grid ytics\n");
    fprintf(fp, "set yrange [%d:%d]\n", (int)data->temp_min - 5, (int)data->temp_max + 5);
    fprintf(fp, "plot '-' using 2:xtic(1) with boxes linecolor rgb '#FF6B6B' notitle\n");
    fprintf(fp, "\"Min\" %.2f\n", data->temp_min);
    fprintf(fp, "\"Current\" %.2f\n", data->temp);
    fprintf(fp, "\"Feels\" %.2f\n", data->feels_like);
    fprintf(fp, "\"Max\" %.2f\n", data->temp_max);
    fprintf(fp, "e\n\n");
    
    // Humidity/Clouds chart
    fprintf(fp, "# Humidity & Clouds\n");
    fprintf(fp, "set title 'Humidity & Cloud Cover'\n");
    fprintf(fp, "set ylabel 'Percentage (%%  )'\n");
    fprintf(fp, "set yrange [0:100]\n");
    fprintf(fp, "plot '-' using 2:xtic(1) with boxes linecolor rgb '#4ECDC4' notitle\n");
    fprintf(fp, "\"Humidity\" %d\n", data->humidity);
    fprintf(fp, "\"Clouds\" %d\n", data->clouds);
    fprintf(fp, "e\n\n");
    
    // Wind speed
    fprintf(fp, "# Wind Speed\n");
    fprintf(fp, "set title 'Wind Speed'\n");
    fprintf(fp, "set ylabel 'Speed (m/s)'\n");
    fprintf(fp, "set yrange [0:10]\n");
    fprintf(fp, "plot '-' using 1:2 with impulses linewidth 15 linecolor rgb '#95E1D3' notitle\n");
    fprintf(fp, "0.5 %.2f\n", data->wind_speed);
    fprintf(fp, "e\n\n");
    
    // Pressure
    fprintf(fp, "# Pressure\n");
    fprintf(fp, "set title 'Pressure Levels'\n");
    fprintf(fp, "set ylabel 'Pressure (hPa)'\n");
    fprintf(fp, "set yrange [%d:%d]\n", data->grnd_level - 10, data->pressure + 10);
    fprintf(fp, "plot '-' using 2:xtic(1) with boxes linecolor rgb '#F38181' notitle\n");
    fprintf(fp, "\"Surface\" %d\n", data->pressure);
    fprintf(fp, "\"Sea Lvl\" %d\n", data->sea_level);
    fprintf(fp, "\"Ground\" %d\n", data->grnd_level);
    fprintf(fp, "e\n\n");
    
    fprintf(fp, "unset multiplot\n");
    fclose(fp);
    
    printf("✅ Gnuplot script created: weather_plot.gnu\n");
    printf("   Run with: gnuplot weather_plot.gnu\n");
}

// Generate CSV for easy import
void generate_csv(WeatherData *data) {
    FILE *fp = fopen("weather_data.csv", "w");
    if (!fp) {
        printf("❌ Error creating CSV file!\n");
        return;
    }
    
    fprintf(fp, "Metric,Value,Unit\n");
    fprintf(fp, "City,%s,\n", data->city);
    fprintf(fp, "Country,%s,\n", data->country);
    fprintf(fp, "Description,%s,\n", data->description);
    fprintf(fp, "Temperature,%.2f,°C\n", data->temp);
    fprintf(fp, "Feels Like,%.2f,°C\n", data->feels_like);
    fprintf(fp, "Min Temp,%.2f,°C\n", data->temp_min);
    fprintf(fp, "Max Temp,%.2f,°C\n", data->temp_max);
    fprintf(fp, "Humidity,%d,%%\n", data->humidity);
    fprintf(fp, "Pressure,%d,hPa\n", data->pressure);
    fprintf(fp, "Sea Level Pressure,%d,hPa\n", data->sea_level);
    fprintf(fp, "Ground Level Pressure,%d,hPa\n", data->grnd_level);
    fprintf(fp, "Wind Speed,%.2f,m/s\n", data->wind_speed);
    fprintf(fp, "Wind Direction,%d,degrees\n", data->wind_deg);
    fprintf(fp, "Clouds,%d,%%\n", data->clouds);
    fprintf(fp, "Visibility,%.1f,km\n", data->visibility/1000.0);
    fprintf(fp, "Latitude,%.4f,degrees\n", data->lat);
    fprintf(fp, "Longitude,%.4f,degrees\n", data->lon);
    
    fclose(fp);
    printf("✅ CSV file created: weather_data.csv\n");
}

int main(void) {
    CURL *curl;
    CURLcode res;
    char url[512];
    ResponseData response = {NULL, 0};
    WeatherData weather_data = {0};
    
    // URL
    snprintf(url, sizeof(url),
            "https://api.openweathermap.org/data/2.5/weather?q=%s,%s&appid=%s&units=metric",
            CITY, COUNTRY, API_KEY);
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
        
        printf("🌍 Fetching weather data from OpenWeatherMap...\n\n");
        
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "❌ Error: %s\n", curl_easy_strerror(res));
        } else {
            // Parse the JSON response
            if (parse_weather_json(response.data, &weather_data)) {
                printf("✅ Weather data fetched successfully!\n\n");
                
                // Generate all visualizations
                printf("🎨 Generating visualizations...\n\n");
                
                generate_ascii(&weather_data);
                generate_html(&weather_data);
                generate_gnuplot(&weather_data);
                generate_csv(&weather_data);
                
                printf("\n✨ All files generated successfully!\n\n");
                printf("Generated files:\n");
                printf("  📄 weather_dashboard.html - Interactive web dashboard\n");
                printf("  📊 weather_plot.gnu - Gnuplot script\n");
                printf("  📈 weather_data.csv - CSV data export\n\n");
                printf("To view:\n");
                printf("  • Open weather_dashboard.html in your browser\n");
                printf("  • Run: gnuplot weather_plot.gnu (creates weather_charts.png)\n");
                printf("  • Import weather_data.csv into Excel/Sheets\n\n");
            } else {
                printf("❌ Failed to parse weather data!\n");
            }
        }
        
        curl_easy_cleanup(curl);
        free(response.data);
    }
    
    return 0;
}
