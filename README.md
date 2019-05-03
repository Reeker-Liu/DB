# Introduction

> This project is front end part of database-miniDB for the course software engineering

#### Other parts of the database
- https://github.com/rsy56640/xjbDB
- https://github.com/ssyram/NovelRulesTranslator

#### documents and references (maybe helpful)

- https://www.youtube.com/playlist?list=PLSE8ODhjZXjYutVzTeAds8xUt1rcmyT7x

- https://www.bilibili.com/video/av39731185
- https://github.com/pingcap/tidb/tree/master/util/ranger
- http://tatiyants.com/postgres-query-plan-visualization/
- https://docs.faircom.com/doc/SQLExplorer/37578.htm



# Issues

### todo

- //supplement development documentation
- change CreateTableInfo
- check pk nums
- tsl delay check till know tablename
- RetValue need bool?
- //(shelved till check optization)check CHAR and VARCHAR length according to table structure(sometimes cannot check)
- debug/log



### Expansibility

- source of DML op structs should be a class, such as singleTableSource ExprTableSource、JoinTableSource、SubqueryTableSource



### optimization

- optimize expression structure when check visiting



### multi-threading (shelved)

- buffer for table structure needs read&write lock
- among simultaneous execution plans, one drops a table which another plan used it (may pass semantics check)



# Implementation

![1556805209071](C:\Users\TR\AppData\Roaming\Typora\typora-user-images\1556805209071.png)

### lexer

### ast

### visitor

### DDL

### DML

### parse(syntax-directed translation)



# Requirements

> requirements need supporting by other parts

- needed interfaces and structs are in namespace VM in [VM.h](https://github.com/Endless-Fighting/DB/blob/master/DB/src/VM.h)
- the table structure of joined tables should keep the original info about columns, such that for TableA TableB with both PK id, the tmp joined table should support that using TableA.id and TableB.id can fetch the same return value
