drop schema if exists `WSdb`;
create schema if not exists `WSdb`;
drop table if exists `WSdb`.`tblMain`;
create table if not exists `WSdb`.`tblMain` (
  id int unsigned not null auto_increment,
  temperature decimal(5,2) null,
  humidity decimal(5,2) null,
  pressure decimal(5,2) null,
  time datetime default CURRENT_TIMESTAMP(),
  primary key (id)
);
INSERT INTO `WSdb`.`tblMain` (`ID`) VALUES (0);
