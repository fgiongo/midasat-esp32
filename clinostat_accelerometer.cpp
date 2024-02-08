  #include <WiFi.h>
  #include <Wire.h>
  #include <Adafruit_MPU6050.h>
  #include <Adafruit_Sensor.h>
  #include <HTTPClient.h>

  #define I2C_SDA 5
  #define I2C_SCL 6

  // Defina as credenciais da sua rede Wi-Fi
  const char* ssid = "SEU_SSID";
  const char* password = "SUA_SENHA";

  // Defina o URL do servidor web
  const char* server_url = "http://SEU_SERVIDOR_WEB/dados";

  // Crie um objeto para o acelerômetro
  Adafruit_MPU6050 mpu(0x68, I2C_SDA, I2C_SCL);

  // Crie um objeto para realizar requests HTTP
  HTTPClient http;

  void setup() {
    // Inicialize a comunicação serial
    Serial.begin(115200);

    // Conecte-se à rede Wi-Fi
    Serial.println("Conectando à rede Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi conectado!");

    // Inicialize o acelerômetro
    if (!mpu.begin()) {
      Serial.println("Falha ao inicializar o acelerômetro!");
      while (1);
    }
    Serial.println("Acelerômetro inicializado!");
  }

  void loop() {
    // Leia os dados do acelerômetro
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Crie um JSON com os dados do acelerômetro
    String json = "{";
    json += "\"x\": " + String(a.acceleration.x);
    json += ", \"y\": " + String(a.acceleration.y);
    json += ", \"z\": " + String(a.acceleration.z);
    json += "}";

    // Envie os dados para o servidor web
    http.begin(server_url);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(json);

    // Verifique o código de retorno da requisição
    if (httpCode > 0) {
      Serial.println("Dados enviados com sucesso!");
      Serial.println("Código de retorno: " + String(httpCode));
    } else {
      Serial.println("Falha ao enviar dados!");
      Serial.println("Código de retorno: " + String(httpCode));
    }

    // Limpe o objeto HTTP
    http.end();

    delay(100); // Ajusta o intervalo de envio dos dados
  }
