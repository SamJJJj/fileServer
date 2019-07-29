-- MySQL dump 10.13  Distrib 8.0.16, for Linux (x86_64)
--
-- Host: localhost    Database: netdisk
-- ------------------------------------------------------
-- Server version	8.0.16

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
 SET NAMES utf8mb4 ;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `file_info`
--

DROP TABLE IF EXISTS `file_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `file_info` (
  `precode` int(11) DEFAULT '0',
  `code` int(11) NOT NULL AUTO_INCREMENT,
  `filename` char(32) DEFAULT NULL,
  `md5` char(32) DEFAULT NULL,
  `owner` char(20) DEFAULT NULL,
  `filesize` int(11) DEFAULT NULL,
  `filetype` char(1) DEFAULT NULL,
  `exist` int(11) DEFAULT '0',
  PRIMARY KEY (`code`)
) ENGINE=InnoDB AUTO_INCREMENT=70 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `file_info`
--

LOCK TABLES `file_info` WRITE;
/*!40000 ALTER TABLE `file_info` DISABLE KEYS */;
INSERT INTO `file_info` VALUES (0,65,'hello','','sam',4096,'d',0),(65,66,'hello','','sam',4096,'d',0);
/*!40000 ALTER TABLE `file_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `token_info`
--

DROP TABLE IF EXISTS `token_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `token_info` (
  `username` char(20) NOT NULL,
  `token` char(32) NOT NULL,
  PRIMARY KEY (`username`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `token_info`
--

LOCK TABLES `token_info` WRITE;
/*!40000 ALTER TABLE `token_info` DISABLE KEYS */;
/*!40000 ALTER TABLE `token_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user_info`
--

DROP TABLE IF EXISTS `user_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `user_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` char(20) DEFAULT NULL,
  `salt` char(12) NOT NULL,
  `password` char(128) DEFAULT NULL,
  `isLogin` int(11) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_info`
--

LOCK TABLES `user_info` WRITE;
/*!40000 ALTER TABLE `user_info` DISABLE KEYS */;
INSERT INTO `user_info` VALUES (2,'sam','$6$4B8VV2W','$6$4B8VV2W$WI89ciZMl4lmTWRdlMCn4UsLxnIw1MzeVpTU1RUO86cf.SyRFwuAkmmeijV3IOIHCcGgL6z4sn7/LJQlmD.4S0',0),(3,'root','$6$cm8Xj62','$6$cm8Xj62$tBZ3Gm/2pZU8EhYPcwj92GnFRVKHs7xAl.K9vwguGF3hcyy.W3MWBAsJZ3/XuuLMmDncVxYdbKp6Tyj3.x6tm1',0),(4,'zhenxiang','$6$BC70CYC','$6$BC70CYC$sDDrxFwEbJaT4B/lxheEl//A.ssuKiRXGJCm1oQSiN4MnmjTaMzoPSwwkewk8JXqu7h4pU.8Zk.3oPkqUGqI/1',0),(7,'sus','$6$Aze0hc8','$6$Aze0hc8$TSuyKRijjLvqpR3frs33xSxW01nOi0tv2s9PuWpF5rQH6llfMjn44Bb3XN7lRcZs7t0Uu80KNjYs/K23VgOcp0',0);
/*!40000 ALTER TABLE `user_info` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-07-29 14:21:42
