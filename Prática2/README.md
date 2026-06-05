Autores:
- João Pedro Machado de Almeida;
- Nicolas Henrique Fernandes da Silva.
  
Ambiente de Desenvolvimento:
- IDE: Simulador EdSim51DI;
- Linguagem: Assembly.
  
Hardware Necessário:
- Microcontrolador AT89S52;
- Display Alfanumérico LCD 16x2 (Controlador HD44780 ou compatível);
- Displays de 7 Segmentos;
- Teclado Matricial.
  
LCD e Teclado Matricial:
- Varredura de teclado matricial para identificar a tecla pressionada e exibi-la em tempo real no display LCD;
- Implementação de debounce para filtrar ruídos mecânicos e evitar leituras duplicadas;
- O sistema bloqueia e impede novas entradas após o pressionamento da tecla '#'.
  
Display de 7 Segmentos:
- Exibe os 4 primeiros dígitos do RA utilizando 4 displays de 7 segmentos;
- Utiliza a técnica de multiplexação, alternando o acionamento de cada display rapidamente para gerar a ilusão de acendimento simultâneo.
  
Como Executar:
- Para executar no hardware físico, compile o código para gerar o arquivo .hex;
- Utilize um gravador ISP para gravar no microcontrolador.  
