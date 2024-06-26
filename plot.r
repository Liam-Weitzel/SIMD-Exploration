#install.packages("readxl")
library(readxl)
#install.packages("dplyr")
library(dplyr)
#install.packages("tidyverse")
library(tidyverse)
#install.packages("car")
library(car)

Data <- read_xlsx("consolidated_data.xlsx")
Data$library <- factor(Data$library)
Data$benchmark <- factor(Data$benchmark)
colnames(Data)[which(names(Data) == "execution time")] <- "execution_time"

Data_BMAddVectors <- Data %>% filter(benchmark == "BM_AddVectors/1/2/3/4/min_time:0.500/repeats:1000")
Data_BMAddVectors <- Data_BMAddVectors %>% filter(library != "no-vec")

histogram <- ggplot(Data_BMAddVectors, aes(x=execution_time, color=library)) + 
  geom_histogram(binwidth=0.002, aes(y=..density..), fill="white") +
  labs(title="AddVectors", y="Density", x="Execution time in nanoseconds") +
  scale_x_continuous(limits = c(0.3, 1.5))

ggsave("BMAddVectors_hist.png", plot=histogram, width=14, height=5, dpi=300)

res.krus <- kruskal.test(execution_time ~ library, data =  Data_BMAddVectors)
capture.output(res.krus, append = FALSE, file = "BMAddVectors_res.txt")
leveneTest(execution_time ~ library, data =  Data_BMAddVectors)

Data_BMAddVectors$execution_time <- 1.407 / Data_BMAddVectors$execution_time

barchart <- ggplot(data=Data_BMAddVectors, aes(x = library,
                                                       y = execution_time,
                                                       colour = library,
                                                       fill = library,
                                                       group = library)) +
  labs(title="AddVectors", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMAddVectors_bar.png", plot=barchart, width=14, height=5, dpi=300)

aggregate(execution_time ~ library ,data=Data_BMAddVectors,summary)

Data_BMFindInVector <- Data %>% filter(benchmark == "BM_FindInVector/456/4096/3254/min_time:0.500/repeats:1000")
Data_BMFindInVector <- Data_BMFindInVector %>% filter(library != "no-vec")

histogram <- ggplot(Data_BMFindInVector, aes(x=execution_time, color=library)) + 
  geom_histogram(binwidth=0.1, aes(y=..density..), fill="white") +
  labs(title="FindInVector", y="Density", x="Execution time in nanoseconds") +
  scale_x_continuous(limits = c(220, 410))

ggsave("BMFindInVector_hist.png", plot=histogram, width=14, height=5, dpi=300)

res.krus <- kruskal.test(execution_time ~ library, data =  Data_BMFindInVector)
capture.output(res.krus, append = FALSE, file = "BMFindInVector_res.txt")
leveneTest(execution_time ~ library, data =  Data_BMFindInVector)

Data_BMFindInVector$execution_time <- 2535 / Data_BMFindInVector$execution_time

barchart <- ggplot(data=Data_BMFindInVector, aes(x = library,
                                               y = execution_time,
                                               colour = library,
                                               fill = library,
                                               group = library)) +
  labs(title="FindInVector", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMFindInVector_box.png", plot=barchart, width=14, height=5, dpi=300)

aggregate(execution_time ~ library ,data=Data_BMFindInVector,summary)

Data_BMFindInVectorFaster <- Data %>% filter(benchmark == "BM_FindInVectorFaster/456/4096/3254/min_time:0.500/repeats:1000")
Data_BMFindInVectorFaster <- Data_BMFindInVectorFaster %>% filter(library != "no-vec")

histogram <- ggplot(Data_BMFindInVectorFaster, aes(x=execution_time, color=library)) + 
  geom_histogram(binwidth=0.1, aes(y=..density..), fill="white") +
  labs(title="FindInVectorFaster", y="Density", x="Execution time in nanoseconds") +
  scale_x_continuous(limits = c(150, 400))

ggsave("BMFindInVectorFaster_hist.png", plot=histogram, width=14, height=5, dpi=300)

res.krus <- kruskal.test(execution_time ~ library, data = Data_BMFindInVectorFaster)
capture.output(res.krus, append = FALSE, file = "BMFindInVectorFaster_res.txt")
leveneTest(execution_time ~ library, data =  Data_BMFindInVectorFaster)

Data_BMFindInVectorFaster$execution_time <- 2535 / Data_BMFindInVectorFaster$execution_time

barchart <- ggplot(data=Data_BMFindInVectorFaster, aes(x = library,
                                                       y = execution_time,
                                                       colour = library,
                                                       fill = library,
                                                       group = library)) +
  labs(title="FindInVectorFaster", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMFindInVectorFaster_box.png", plot=barchart, width=14, height=5, dpi=300)

aggregate(execution_time ~ library ,data=Data_BMFindInVectorFaster,summary)

Data_BMSumVector <- Data %>% filter(benchmark == "BM_SumVector/0/4096/min_time:0.500/repeats:1000")
Data_BMSumVector <- Data_BMSumVector %>% filter(library != "no-vec")

histogram <- ggplot(Data_BMSumVector, aes(x=execution_time, color=library)) + 
  geom_histogram(binwidth=0.1, aes(y=..density..), fill="white") +
  labs(title="SumVector", y="Density", x="Execution time in nanoseconds") +
  scale_x_continuous(limits = c(105, 285))

ggsave("BMSumVector_hist.png", plot=histogram, width=14, height=5, dpi=300)

res.krus <- kruskal.test(execution_time ~ library, data = Data_BMSumVector)
capture.output(res.krus, append = FALSE, file = "BMSumVector_res.txt")
leveneTest(execution_time ~ library, data =  Data_BMSumVector)

Data_BMSumVector$execution_time <- 1856 / Data_BMSumVector$execution_time

barchart <- ggplot(data=Data_BMSumVector, aes(x = library,
                                                 y = execution_time,
                                                 colour = library,
                                                 fill = library,
                                                 group = library)) +
  labs(title="SumVector", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMSumVector_box.png", plot=barchart, width=14, height=5, dpi=300)
aggregate(execution_time ~ library ,data=Data_BMSumVector,summary)

Data_BMReverseVector <- Data %>% filter(benchmark == "BM_ReverseVector/0/4096/min_time:0.500/repeats:1000")
Data_BMReverseVector <- Data_BMReverseVector %>% filter(library != "no-vec")

histogram <- ggplot(Data_BMReverseVector, aes(x=execution_time, color=library)) + 
  geom_histogram(binwidth=0.1, aes(y=..density..), fill="white") +
  labs(title="ReverseVector", y="Density", x="Execution time in nanoseconds") +
  scale_x_continuous(limits = c(180, 265))

ggsave("BMReverseVector_hist.png", plot=histogram, width=14, height=5, dpi=300)

res.krus <- kruskal.test(execution_time ~ library, data = Data_BMReverseVector)
capture.output(res.krus, append = FALSE, file = "BMReverseVector_res.txt")
leveneTest(execution_time ~ library, data =  Data_BMReverseVector)

Data_BMReverseVector$execution_time <- 1540 / Data_BMReverseVector$execution_time

barchart <- ggplot(data=Data_BMReverseVector, aes(x = library,
                                              y = execution_time,
                                              colour = library,
                                              fill = library,
                                              group = library)) +
  labs(title="ReverseVector", y="Speed up", x="Library") +
  theme_minimal() +
  geom_bar(stat = "summary", fun = "mean")

ggsave("BMReverseVector_box.png", plot=barchart, width=14, height=5, dpi=300)
aggregate(execution_time ~ library ,data=Data_BMReverseVector,summary)

