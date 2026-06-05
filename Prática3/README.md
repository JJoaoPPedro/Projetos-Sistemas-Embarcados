Autores:
- João Pedro Machado de Almeida;
- Nicolas Henrique Fernandes da Silva.
  
Ambiente de Desenvolvimento:
- IDE: Simulador EdSim51DI;
- Linguagem: Assembly.
  
Hardware Necessário:
- Microcontrolador AT89S52;
- Display Alfanumérico LCD 16x2 (Controlador HD44780 ou compatível);
- Teclado Matricial.
  
Cofre:
- Exibição de uma mensagem na primeira linha do LCD solicitando a entrada da senha de 4 dígitos;
- Varredura de teclado matricial para receber a senha, exibindo um caractere '*' no LCD para cada dígito inserido;
- Implementação de debounce para filtrar ruídos mecânicos e evitar leituras duplicadas;
- Compara os valores digitados pelo usuário com os valores de uma senha previamente armazenada na memória (1234);
- Exibe uma mensagem de "ok" na segunda linha do LCD caso a senha seja correta, ou uma mensagem de erro caso contrário.
  
Como Executar:
- Para executar no hardware físico, compile o código para gerar o arquivo .hex;
- Utilize um gravador ISP para gravar no microcontrolador.
