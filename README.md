/* 
 * MO815 - Análise de Imagem Orientada a um Problema do Mundo Real
 *
 * Detecção de placas
 *
 * Autores: Amadeu Bonfante
 *	  : Luis Claudio Sugi Afonso
 *	  : Luiz Antonio Falaguasta Barbosa
 *
 */

- Dependências: É necessário a instalação de todas as dependências para o funcionamento da libift, as quais são:
	libsvm-tools liblapack-dev libblas-dev libatlas-base-dev
	As dependências podem ser instaladas via apt-get no Ubuntu.

- Para executar o programa de detecção de placas é necessário realizar a compilação do código que se encontra na pasta 'demo'. Entre na pasta via terminal e digite o seguinte comando:
	make iftSelectCandidates 
    
- O executável será gerado na pasta 'bin' com mesmo nome do programa compilado.

- O usuário deve fornecer 4 parâmetros de entrada para o executável:
	<P1>: diretório onde se encontram as imagens em tons de cinza para que a detecção de placas seja realizada
	<P2>: diretório onde as imagens com as placas detectadas serão salvas
	<P3>: método de binarização que pode ser: (0) Sauvola - (1) Niblack - (2) Otsu
	<P4>: utilização de pooling: (0) Não - (1) Sim

por exemplo:
	./iftSelectCandidates ../imgs/ ../candidates/ 0 0
em que:
	<P1>: ../imgs/
	<P2>: ../candidates/
	<P3>: 0
	<P4>: 0

