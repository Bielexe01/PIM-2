-- Sistema Apollo – Catálogo de Agentes de IA (PIM II, UNIP, 2026)
-- Script do banco de dados: criação das tabelas, dados de exemplo e consultas úteis

-- Criação das tabelas
CREATE TABLE categoria (
    id_categoria INT NOT NULL PRIMARY KEY,
    nome         VARCHAR(40) NOT NULL UNIQUE );
CREATE TABLE agente (
    id_agente     INT NOT NULL PRIMARY KEY,
    nome          VARCHAR(80) NOT NULL,
    instrucoes    VARCHAR(1000) NOT NULL,
    nota          DECIMAL(2,1) NOT NULL DEFAULT 0.0,
    id_categoria  INT NOT NULL,
    data_cadastro DATE NOT NULL,
    CONSTRAINT ck_nota CHECK (nota BETWEEN 0 AND 5),
    FOREIGN KEY (id_categoria) REFERENCES categoria (id_categoria) );
CREATE TABLE tag (
    id_tag INT NOT NULL PRIMARY KEY,
    nome   VARCHAR(30) NOT NULL UNIQUE );
CREATE TABLE agente_tag (
    id_agente INT NOT NULL,
    id_tag    INT NOT NULL,
    PRIMARY KEY (id_agente, id_tag),
    FOREIGN KEY (id_agente) REFERENCES agente (id_agente) ON DELETE CASCADE,
    FOREIGN KEY (id_tag) REFERENCES tag (id_tag) );

-- Dados de exemplo
INSERT INTO categoria VALUES (1,'Atendimento'), (2,'Financeiro'),
  (3,'Desenvolvimento'), (4,'Marketing'), (5,'Recursos Humanos'), (6,'Outros');
INSERT INTO agente VALUES
 (1,'Agente de Triagem','Classifica chamados por urgência e encaminha para o setor responsável.',4.5,1,'2026-10-06'),
 (2,'Agente de Atendimento','Responde dúvidas frequentes dos clientes sobre produtos e prazos.',3.0,2,'2026-10-07'),
 (3,'Agente de Código','Sugere correções de código C e aponta erros de lógica.',5.0,3,'2026-10-08'),
 (4,'Redator de Posts Institucionais','Use linguagem inclusiva...',3.7,4,'2026-10-09'),
 (5,'Triagem de Currículos','Desconsidere nome, idade, gênero...',3.9,5,'2026-10-12'),
 (6,'Gerador de Casos de Teste','Você propõe casos de teste...',4.4,3,'2026-10-13');
INSERT INTO tag VALUES (1,'suporte'),(2,'cliente'),(3,'infra'),(4,'auditoria'),
 (5,'codigo'),(6,'linguagem-c'),(7,'redes-sociais'),(8,'recrutamento'),(9,'testes');
INSERT INTO agente_tag VALUES (1,1),(1,2),(2,2),(2,4),(3,5),(3,6),(4,7),(5,8),(6,9),(6,5);

-- Consultas úteis
-- Listagem com categoria
SELECT a.id_agente, a.nome, c.nome, a.nota FROM agente a
  JOIN categoria c ON c.id_categoria = a.id_categoria ORDER BY a.id_agente;
 
-- Busca por ID
SELECT id_agente, nome, nota FROM agente WHERE id_agente = 3;
 
-- Busca por nome (parte do nome)
SELECT id_agente, nome, nota FROM agente WHERE nome LIKE '%Código%';
 
-- Agentes com nota igual ou superior a 4, da maior para a menor
SELECT id_agente, nome, nota FROM agente WHERE nota >= 4.0 ORDER BY nota DESC;
 
-- Quantidade e média de nota por categoria
SELECT c.nome, COUNT(*) AS qtd, ROUND(AVG(a.nota), 2) AS media
  FROM categoria c JOIN agente a ON a.id_categoria = c.id_categoria
 GROUP BY c.nome ORDER BY media DESC;
