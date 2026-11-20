// =====================================
// BENGALA ELETRONICA ASSISTIVA
// MODO NORMAL E MODO SILENCIOSO
// =====================================


// PINOS
const int buzzer = 13;
const int botao = 12;
const int trig = 11;
const int echo = 10;


// SENSOR
long duracao;
float distancia;
float ultimaDistanciaValida = 0;


bool possuiLeituraValida = false;


// MODOS
bool modoNormal = true;


// BOTAO
int estadoBotao = HIGH;
int ultimaLeituraBotao = HIGH;


unsigned long tempoDebounce = 0;
const unsigned long debounce = 50;


// BUZZER
bool buzzerLigado = false;
unsigned long tempoBuzzer = 0;


int frequenciaBuzzer = 0;
int intervaloBuzzer = 0;




// =====================================
// SETUP
// =====================================


void setup()
{
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);


  pinMode(buzzer, OUTPUT);
  pinMode(botao, INPUT_PULLUP);


  Serial.begin(9600);


  digitalWrite(trig, LOW);
  noTone(buzzer);


  Serial.println("Sistema iniciado!");
  Serial.println("Modo normal ativado");
}




// =====================================
// BOTAO
// =====================================


void verificarBotao()
{
  int leitura = digitalRead(botao);


  if (leitura != ultimaLeituraBotao)
  {
    tempoDebounce = millis();
  }


  if (millis() - tempoDebounce >= debounce)
  {
    if (leitura != estadoBotao)
    {
      estadoBotao = leitura;


      if (estadoBotao == LOW)
      {
        modoNormal = !modoNormal;


        if (modoNormal)
        {
          Serial.println("Modo normal ativado");
        }
        else
        {
          Serial.println("Modo silencioso ativado");
          desligarBuzzer();
        }
      }
    }
  }


  ultimaLeituraBotao = leitura;
}




// =====================================
// SENSOR ULTRASSONICO
// =====================================


void lerSensor()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);


  digitalWrite(trig, HIGH);
  delayMicroseconds(10);


  digitalWrite(trig, LOW);


  duracao = pulseIn(echo, HIGH, 25000);


  if (duracao == 0)
  {
    return;
  }


  distancia = duracao * 0.034 / 2;


  if (distancia < 2 || distancia > 400)
  {
    return;
  }


  ultimaDistanciaValida = distancia;
  possuiLeituraValida = true;


  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");
}




// =====================================
// DESLIGAR BUZZER
// =====================================


void desligarBuzzer()
{
  noTone(buzzer);


  buzzerLigado = false;
}




// =====================================
// CONTROLE DO BUZZER
// =====================================


void controlarBuzzer()
{
  if (!modoNormal || frequenciaBuzzer == 0)
  {
    desligarBuzzer();
    return;
  }


  unsigned long agora = millis();


  // BUZZER DESLIGADO: AGUARDA PARA APITAR
  if (!buzzerLigado)
  {
    if (agora - tempoBuzzer >= intervaloBuzzer)
    {
      tone(buzzer, frequenciaBuzzer);


      buzzerLigado = true;
      tempoBuzzer = agora;
    }
  }


  // BUZZER LIGADO: DESLIGA APOS 100 MS
  else
  {
    if (agora - tempoBuzzer >= 100)
    {
      noTone(buzzer);


      buzzerLigado = false;
      tempoBuzzer = agora;
    }
  }
}




// =====================================
// CONTROLE DO SISTEMA
// =====================================


void controlarSistema()
{
  if (!possuiLeituraValida)
  {
    frequenciaBuzzer = 0;
    desligarBuzzer();


    return;
  }


  int frequencia = 0;
  int intervalo = 0;


  // ATE 10 CM
  if (ultimaDistanciaValida <= 10)
  {
    frequencia = 1800;
    intervalo = 120;
  }


  // 10 A 30 CM
  else if (ultimaDistanciaValida <= 30)
  {
    frequencia = 1600;
    intervalo = 200;
  }


  // 30 A 50 CM
  else if (ultimaDistanciaValida <= 50)
  {
    frequencia = 1400;
    intervalo = 300;
  }


  // 50 A 100 CM
  else if (ultimaDistanciaValida <= 100)
  {
    frequencia = 1200;
    intervalo = 450;
  }


  // ACIMA DE 100 CM
  else
  {
    frequencia = 0;
    intervalo = 0;
  }


  // ATUALIZA BUZZER QUANDO MUDA A DISTANCIA
  if (frequencia != frequenciaBuzzer ||
      intervalo != intervaloBuzzer)
  {
    frequenciaBuzzer = frequencia;
    intervaloBuzzer = intervalo;


    desligarBuzzer();
    tempoBuzzer = millis();
  }


  controlarBuzzer();
}




// =====================================
// LOOP PRINCIPAL
// =====================================


void loop()
{
  verificarBotao();
  lerSensor();
  controlarSistema();
}