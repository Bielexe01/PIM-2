-- ============================================================================
-- PIM II - Sistema de Catalogo e Gerenciador de Configuracoes de Agentes de IA
-- Script SQL de criacao das estruturas, dados de exemplo e consultas uteis
-- (item 2.5 das Orientacoes do PIM II)
--
-- Compatibilidade: SQL padrao (MySQL 8, MariaDB, PostgreSQL, SQL Server).
-- As consultas marcadas com [MySQL] usam LIMIT; nas demais troque por
-- TOP n (SQL Server) ou FETCH FIRST n ROWS ONLY (PostgreSQL).
--
-- Esta modelagem representa os mesmos dados que o programa em C guarda nos
-- quatro vetores paralelos + arquivo agentes.txt. A tabela tem uma coluna por
-- vetor e uma linha por agente:
--
--     vetor em C          coluna na tabela
--     ids[]      (int)    agentes.id
--     nomes[]    (char)   agentes.nome
--     scripts[]  (char)   agentes.instrucoes
--     notas[]    (float)  agentes.nota
-- ============================================================================


-- ----------------------------------------------------------------------------
-- 1. CRIACAO DA ESTRUTURA
-- ----------------------------------------------------------------------------
-- Em MySQL/MariaDB, crie e selecione o banco antes (descomente se necessario):
-- CREATE DATABASE catalogo_agentes CHARACTER SET utf8mb4;
-- USE catalogo_agentes;

DROP TABLE IF EXISTS agentes;

CREATE TABLE agentes (
    -- Identificador unico do agente. E a chave primaria porque o manual exige
    -- "identificador unico" (item 2.6). No programa em C essa regra e garantida
    -- pela funcao buscarindiceporid() antes de qualquer cadastro.
    id           INT            NOT NULL,

    -- Nome do agente: ate 80 caracteres (item 2.6).
    -- VARCHAR(80) ja conta os 80 caracteres. No C o vetor e char[81] porque
    -- precisa de 1 posicao extra para o terminador '\0' (Aula 6).
    nome         VARCHAR(80)    NOT NULL,

    -- Script/instrucoes: ate 1000 caracteres, incluindo espacos, em uma unica
    -- entrada textual (item 2.6). No C o vetor e char[1001] pelo mesmo motivo.
    instrucoes   VARCHAR(1000)  NOT NULL,

    -- Nota de 0 a 5. A restricao CHECK repete no banco a mesma validacao que a
    -- funcao notavalida() faz no programa em C.
    nota         DECIMAL(2,1)   NOT NULL,

    -- Uma linha por agente, como no arquivo agentes.txt.
    CONSTRAINT pk_agentes      PRIMARY KEY (id),
    CONSTRAINT ck_agentes_nome CHECK (CHAR_LENGTH(TRIM(nome)) > 0),
    CONSTRAINT ck_agentes_instr CHECK (CHAR_LENGTH(TRIM(instrucoes)) > 0),
    CONSTRAINT ck_agentes_nota CHECK (nota >= 0 AND nota <= 5)
);

-- Observacao sobre o limite de 50 agentes:
-- o limite e uma regra da aplicacao em C (constante MAXAGENTES), nao do banco.
-- Se quiser garantir o limite tambem no banco, use um TRIGGER BEFORE INSERT que
-- conte as linhas e cancele acima de 50. Fica fora do script basico por nao ser
-- exigido pelo manual.


-- ----------------------------------------------------------------------------
-- 2. DADOS DE EXEMPLO
-- ----------------------------------------------------------------------------
-- Os mesmos tres agentes usados nos testes do programa em C.
INSERT INTO agentes (id, nome, instrucoes, nota) VALUES
    (101, 'Agente de Triagem',    'Classifica chamados por urgencia.',   4.5),
    (102, 'Agente de Atendimento','Responde duvidas frequentes.',        3.0),
    (103, 'Agente de Codigo',     'Sugere correcoes de codigo C.',       5.0);

-- Mais dois registros para as consultas por nota e por nome ficarem melhores.
INSERT INTO agentes (id, nome, instrucoes, nota) VALUES
    (104, 'Agente de Revisao',    'Revisa textos e aponta erros de portugues.', 3.5),
    (105, 'Agente Financeiro',    'Apoia a analise de despesas mensais.',       4.0);


-- ----------------------------------------------------------------------------
-- 3. CONSULTAS UTEIS (item 2.5)
-- ----------------------------------------------------------------------------

-- 3.1 Listagem de agentes (equivale a opcao 2 do menu do programa em C)
SELECT id, nome, instrucoes, nota
FROM   agentes
ORDER  BY id;

-- 3.2 Busca por ID (equivale a opcao 3 - busca exata, retorna no maximo 1 linha)
SELECT id, nome, instrucoes, nota
FROM   agentes
WHERE  id = 101;

-- 3.3 Busca por nome (equivale a opcao 4 - busca por parte do nome)
--     O % antes e depois significa "qualquer texto em volta".
--     Em MySQL/MariaDB a comparacao de texto ja ignora maiusculas por padrao;
--     no programa em C isso e feito pela funcao contem(), que compara letra a
--     letra em minusculo (nao foi usada strstr, que nao esta nas Aulas 2 a 7).
SELECT id, nome, nota
FROM   agentes
WHERE  nome LIKE '%Agente%'
ORDER  BY nome;

-- 3.4 Selecao por nota (nota minima)
SELECT id, nome, nota
FROM   agentes
WHERE  nota >= 4.0
ORDER  BY nota DESC, nome;

-- 3.5 Diferencial do projeto: listagem ordenada por nota, maior primeiro
--     (equivale a opcao 5 do menu do programa em C)
SELECT id, nome, nota
FROM   agentes
ORDER  BY nota DESC, id;

-- 3.6 Consultas de apoio (contagens e medias) - uteis para o texto e para os testes
SELECT COUNT(*)                 AS total_de_agentes,
       ROUND(AVG(nota), 2)      AS nota_media,
       MIN(nota)                AS menor_nota,
       MAX(nota)                AS maior_nota
FROM   agentes;

-- 3.7 Quantos agentes existem para cada nota (distribuicao)
SELECT nota, COUNT(*) AS quantidade
FROM   agentes
GROUP  BY nota
ORDER  BY nota DESC;

-- 3.8 Verificacao de integridade: nomes repetidos (o manual exige ID unico,
--     mas nomes podem se repetir; esta consulta mostra se ha duplicidade)
SELECT nome, COUNT(*) AS vezes
FROM   agentes
GROUP  BY nome
HAVING COUNT(*) > 1;


-- ----------------------------------------------------------------------------
-- 4. OPERACOES DE MANUTENCAO (equivalentes as opcoes Editar e Excluir)
-- ----------------------------------------------------------------------------

-- 4.1 Editar o nome e a nota do agente 102 (equivale a funcao editar() em C)
UPDATE agentes
SET    nome = 'Agente de Atendimento v2',
       nota = 3.8
WHERE  id = 102;

-- 4.2 Excluir o agente 104 (equivale a funcao excluir() em C)
--     No programa em C a exclusao tambem "puxa" os vetores para tras para
--     manter o mesmo indice nas quatro estruturas. No banco isso nao e
--     necessario: a linha simplesmente deixa de existir.
DELETE FROM agentes
WHERE  id = 104;

-- 4.3 Recriar o agente 102 com o valor original (para deixar o script repetivel)
UPDATE agentes
SET    nome = 'Agente de Atendimento',
       nota = 3.0
WHERE  id = 102;


-- ----------------------------------------------------------------------------
-- 5. DICIONARIO DE DADOS (para copiar na tabela do capitulo 2.5)
-- ----------------------------------------------------------------------------
-- Campo        Tipo            Nulo  Restricao                  Descricao
-- id           INT             Nao   PK; unico; > 0             Identificador unico do agente
-- nome         VARCHAR(80)     Nao   nao vazio                  Nome do agente (ate 80 caracteres)
-- instrucoes   VARCHAR(1000)   Nao   nao vazio                  Script/instrucoes em texto unico
-- nota         DECIMAL(2,1)    Nao   0 <= nota <= 5             Nota de avaliacao do agente
--
-- Entidades e relacionamentos: existe uma unica entidade (agentes) e nenhum
-- relacionamento obrigatorio, portanto nao ha chave estrangeira. Se o grupo
-- quiser evoluir a modelagem, a evolucao natural e criar uma tabela
-- "categorias" e uma coluna categoria_id em agentes (1:N). Isso pode ser
-- citado como melhoria futura, sem substituir o DER minimo acima.
