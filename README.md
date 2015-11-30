/* 
 * MO815 - Análise de Imagem Orientada a um Problema do Mundo Real
 *
 * Detecção de placas
 *
 * Autores: Amadeu Bonfante
 *	  : Hugo Oshiro
 *	  : Luis Claudio Sugi Afonso
 *	  : Luiz Antonio Falaguasta Barbosa
 *
 */

- Dependências: É necessário a instalação de todas as dependências para o funcionamento da libift, as quais são:
	libsvm-tools liblapack-dev libblas-dev libatlas-base-dev
	As dependências podem ser instaladas via apt-get no Ubuntu.

- Para executar o programa de detecção de placas é necessário realizar a compilação do código que se encontra na pasta 'demo'. Entre na pasta via terminal e digite os seguintes comandos:
	make recognizePlate 
	make charExtraction
    
- A execução deverá ser realizada utilizando o arquivo bash 'recognizePlate.sh' que se encontra na pasta 'demo'.

- O usuário deve fornecer 2 parâmetros de entrada para o executável:
	<P1>: diretório onde se encontram as imagens em tons de cinza.
	<P2>: diretório em que são salvas as imagens de placa segmentadas e com orientação corrigida

por exemplo:
	./recognizePlate.sh ../imgs/ ../plates/
em que:
	<P1>: ../imgs/
	<P2>: ../plates/

- Obs.: Os caracteres detectados serão todos salvos na pasta 'caracteres'. Na pasta serão encontradas iamgens com dois padrões de nomes: <num0>_char_<num1>.pgm e <num0>_labeled.pgm. <num0> refere-se ao número da imagem de placa original e <num1> é referente ao identificador do caracter encontrado, sendo que o identificador não é o rótulo do caracter.


