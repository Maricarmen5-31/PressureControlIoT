#include <pgmspace.h>
 
#define SECRET

// Configuración de red WiFi 
const char WIFI_SSID[] = "Micaela";                     //MICAELA
const char WIFI_PASSWORD[] = "zxcvbnmpoiu45";           //zxcvbnmpoiu45

// Definición del nombre del dispositivo IoT 
#define THINGNAME "ESP8266"
 
// Configuración de zona horaria
int8_t TIME_ZONE = -5; //NYC(USA): -5 UTC

// Configuración del host MQTT 
const char MQTT_HOST[] = "a14yncwleucddp-ats.iot.us-east-2.amazonaws.com";
 
// AmazonRootCA1
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
 
// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUJO/aOUdxpuVRhKeClZ+uUGOY1oUwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMDcwMjIxNTU1
M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMmmxqzDPSUuoRkOm/vM
4mevjTPPWuxS1I8owJNaAde1sJQarvF5u/9Jh+A9dWYM6z5mSFA8fMq79/fK2LSl
Ar74qg2rWHq/ewpDZkFQ/lmcWvqgbopHRaZElKcSAt96Pp23SLe+2yM7nssmsbGl
/uffk73X/vW483IFFKQ7EJP3RKQCFnA2uGMpV9bK8+hOgUW8z+2QFmj7aG73fQHS
hNtzUAiGlMdU8kDmaw03G+IeggLAzeuaxz0y6xRkso02eXVa2PYLAZeXiHEQG4O0
VWBaIgp02IbORGY+ev1d4EV8WZ/RrM1SOkaun87EqegkUJGAegE456HvOHK0kxyz
krkCAwEAAaNgMF4wHwYDVR0jBBgwFoAUWR3vI7vTKNkcZEELyncCQNf+XWUwHQYD
VR0OBBYEFNY2ylFEctjgAvbrBXb0i+iI5FVLMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQARK3iS6r6Rk3FLJx39bERkUEnU
emB/az+x9L47Me4bsWuWZvZYvgt+y+df7Rga5ezpWrGx3vp3bUYFBNtpyuLeHEWQ
+Eqw8oEAPZPSi5FaKROkyUlaIl3/2lF1Unarnmj3g/wvVKxHV1Udvn4AT1k5FGpd
tRFBcXN4YE8YBslX8m/eY1EGg52XthYOUesNTlKIkwFvrCWqlIsokWQddIOG+ZuY
C9X+1GViYOLzlmUaLW3ocDkbGoo0lGGkoQHAGddi75e096YkSaD9PNSyFmHgzKoM
ucTrqLBCmhpHIkCvXcluSMwGGDexXrjy03g0HHk9N2ntvQU61mEVF4m9fkq4
-----END CERTIFICATE-----
)KEY";
 
 
// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAyabGrMM9JS6hGQ6b+8ziZ6+NM89a7FLUjyjAk1oB17WwlBqu
8Xm7/0mH4D11ZgzrPmZIUDx8yrv398rYtKUCvviqDatYer97CkNmQVD+WZxa+qBu
ikdFpkSUpxIC33o+nbdIt77bIzueyyaxsaX+59+Tvdf+9bjzcgUUpDsQk/dEpAIW
cDa4YylX1srz6E6BRbzP7ZAWaPtobvd9AdKE23NQCIaUx1TyQOZrDTcb4h6CAsDN
65rHPTLrFGSyjTZ5dVrY9gsBl5eIcRAbg7RVYFoiCnTYhs5EZj56/V3gRXxZn9Gs
zVI6Rq6fzsSp6CRQkYB6ATjnoe84crSTHLOSuQIDAQABAoIBABRPiQlTrO/2z2Ef
uJRXIaLB6t4CD61n9mLo/uSYl0Dr2vo/6ckGsunWtSnf5tg9iuBBGx2Byiv9XM+H
jeY8DJfBohc4X/QVH0Z8XWtMxiGufvdpe1jQLvK9S5+8T6/Et1PZZyiiv52/3pGl
txYmmxMt0Mq5W8PSOisM2fRkwpm0izFyzKkySkuIbff803ldReQ1whoLwxVmHFZR
OksV7MUcBCikVKNRWpus3un2NFpztzoCMe0uFbO9YeKOcRIT9iBNGC8etbnIJ4Q8
tRczlnbYtk1s1H8L8r3b/KjCpiSLKC9RCaeL8BamRFHFctx15vCiegCDSbRHcNzH
Agvf5NECgYEA+DSb6p/TGoE9bMO1AqJyPkZq/1WCYXe/tFrcYJyg4pWNdKG68G+Z
gfV+TRQpopL31RLevNnPiJ3wOzclvMyVqda5EK0selvO4mPDTc+1GYQVCCD95tQ0
LQdxIOpldM+OJojAyafkIuy9q718ScHiIRLVzJw58USsP3EH+ZSrIn0CgYEAz/vo
Fix/jzrSEer+OpBBC9SiWlrwOjrVe/uwq19UT4LLPf+nuA6+qRH9ozv8W9kif+IP
cbMgahqBpQvjVSLSrT+lcqVKdu0Pm66V7wU4D/ssHq1FEtKAlLihvaMmYVTCnn91
X2GO1l5jKudeTTozoHIpqeeQTxNWZaBgDQdbSe0CgYEAq5EJ+Xtym86cG/OHyoNp
fryEN2zvC9qPi+aGkM2TtCv2NJmas5ogvSFppXP7d6IpZ4J8roPMnb22kegD0OaF
yXlgQTPDG+0cK4cUOFQL9dBEDXNorkcercWIIF2FteHGe8JpqLrBypZkYTNAl2dK
nNv9J0sSDkEO42eJQK7QQK0CgYEAwK5kfh5tcpkQws7NBh5cVISCgGjdQ2T8IKe1
LZ7ccej/N4ngrLDkS6fCLfbFvaW0w3r/ydskPYge8XPJATeIrD5ZnZOqH/FRU25z
swNHw+5zGlx1sJFZ0+4U0aYTSXqH7uSwLVQpS0oYX0gH4k34RD1fgetokwmcMfxY
39kf5qUCgYEAxsY8xtYy29CC/lHSCwtSnjpn58r9lQe5wWN2P4RR6J39yR3OWWRV
U7L/Z3/jI2Eu0kCTuFeoj6CKpLUdd6ibJHBUxnkWaI3kxp/+G1GInw0f/pi18YGC
By0WSMZYfP1FvJXa5m3VD6M4IL3pFgd7+n2KGm8cE+LB+s/4RLaR7yY=
-----END RSA PRIVATE KEY-----
)KEY";