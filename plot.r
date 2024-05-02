#install.packages('readxl')
library(readxl)
#install.packages("dplyr")
library(dplyr)
#install.packages("tidyverse")
library(tidyverse)
Data <- read_xlsx("consolidated_data.xlsx")

#summary(Data)
Data$library <- factor(Data$library)
Data$benchmark <- factor(Data$benchmark)
colnames(Data)[which(names(Data) == "execution time")] <- "execution_time"

Data_BMAddVectors <- Data %>% filter(benchmark == "BM_AddVectors/1/2/3/4/min_time:0.500/repeats:1000")
Data_BMAddVectors$execution_time <- 1.407 / Data_BMAddVectors$execution_time
Data_BMAddVectors <- Data_BMAddVectors %>% filter(library != "no-vec")

barchart <- ggplot(data=Data_BMAddVectors, aes(x = library,
                                                       y = execution_time,
                                                       colour = library,
                                                       fill = library,
                                                       group = library)) +
  labs(title="AddVectors", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMAddVectors.png", plot=barchart, width=14, height=5, dpi=300)

Data_BMFindInVector <- Data %>% filter(benchmark == "BM_FindInVector/456/4096/3254/min_time:0.500/repeats:1000")
Data_BMFindInVector$execution_time <- 2535 / Data_BMFindInVector$execution_time
Data_BMFindInVector <- Data_BMFindInVector %>% filter(library != "no-vec")

barchart <- ggplot(data=Data_BMFindInVector, aes(x = library,
                                               y = execution_time,
                                               colour = library,
                                               fill = library,
                                               group = library)) +
  labs(title="FindInVector", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMFindInVector.png", plot=barchart, width=14, height=5, dpi=300)

Data_BMFindInVectorFaster <- Data %>% filter(benchmark == "BM_FindInVectorFaster/456/4096/3254/min_time:0.500/repeats:1000")
Data_BMFindInVectorFaster$execution_time <- 2535 / Data_BMFindInVectorFaster$execution_time
Data_BMFindInVectorFaster <- Data_BMFindInVectorFaster %>% filter(library != "no-vec")

barchart <- ggplot(data=Data_BMFindInVectorFaster, aes(x = library,
                                                       y = execution_time,
                                                       colour = library,
                                                       fill = library,
                                                       group = library)) +
  labs(title="FindInVectorFaster", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMFindInVectorFaster.png", plot=barchart, width=14, height=5, dpi=300)

Data_BMSumVector <- Data %>% filter(benchmark == "BM_SumVector/0/4096/min_time:0.500/repeats:1000")
Data_BMSumVector$execution_time <- 1856 / Data_BMSumVector$execution_time
Data_BMSumVector <- Data_BMSumVector %>% filter(library != "no-vec")

barchart <- ggplot(data=Data_BMSumVector, aes(x = library,
                                                 y = execution_time,
                                                 colour = library,
                                                 fill = library,
                                                 group = library)) +
  labs(title="SumVector", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMSumVector.png", plot=barchart, width=14, height=5, dpi=300)

Data_BMReverseVector <- Data %>% filter(benchmark == "BM_ReverseVector/0/4096/min_time:0.500/repeats:1000")
Data_BMReverseVector$execution_time <- 1540 / Data_BMReverseVector$execution_time
Data_BMReverseVector <- Data_BMReverseVector %>% filter(library != "no-vec")

barchart <- ggplot(data=Data_BMReverseVector, aes(x = library,
                                              y = execution_time,
                                              colour = library,
                                              fill = library,
                                              group = library)) +
  labs(title="ReverseVector", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMReverseVector.png", plot=barchart, width=14, height=5, dpi=300)
