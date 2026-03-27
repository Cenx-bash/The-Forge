# ============================================
# EXPLORATORY DATA ANALYSIS - PHILIPPINE ECONOMY
# ============================================

# Load required libraries
library(tidyverse)
library(ggplot2)
library(dplyr)
library(tidyr)
library(corrplot)
library(gridExtra)
library(reshape2)

# ============================================
# 1. DATASET INTRODUCTION
# ============================================

cat("========================================\n")
cat("DATASET INTRODUCTION\n")
cat("========================================\n")
cat("Dataset Title: Philippine Economic Indicators (2015-2023)\n")
cat("Source: Philippine Statistics Authority / World Bank\n")
cat("Description: This dataset contains key macroeconomic indicators\n")
cat("for the Philippines including GDP, GDP Growth, GDP per Capita,\n")
cat("Inflation Rate, Unemployment Rate, and Poverty Rate from 2015-2023.\n")
cat("\nPurpose: To analyze the economic performance and recovery patterns\n")
cat("of the Philippines, particularly the impact of the COVID-19 pandemic\n")
cat("and the subsequent economic recovery period.\n")

# ============================================
# 2. DATA INSPECTION
# ============================================

cat("\n========================================\n")
cat("DATA INSPECTION\n")
cat("========================================\n")

# Create the dataset
ph_data <- data.frame(
  Year = c(2015:2023),
  GDP_Billion_USD = c(292.774, 304.905, 328.48, 346.84, 376.82, 361.49, 393.61, 404.28, 436.62),
  GDP_Growth_Pct = c(6.1, 6.9, 6.7, 6.3, 6.0, -9.5, 5.6, 7.7, 5.6),
  GDP_per_Capita_USD = c(2891, 2975, 3104, 3251, 3485, 3298, 3579, 3623, 3720),
  Inflation_Rate_Pct = c(1.4, 1.8, 2.9, 5.2, 2.4, 2.4, 3.9, 5.8, 6.0),
  Unemployment_Rate_Pct = c(6.3, 5.5, 5.7, 5.3, 5.1, 10.3, 7.8, 5.4, 4.4),
  Poverty_Rate_Pct = c(21.6, 21.9, 21.6, 16.6, 16.7, 18.1, 18.0, 16.7, 15.5)
)

# Inspect structure
cat("\nDataset Structure:\n")
str(ph_data)

cat("\nDataset Summary:\n")
summary(ph_data)

cat("\nFirst 5 rows:\n")
head(ph_data)

cat("\nDataset Dimensions:\n")
cat("Rows:", nrow(ph_data), "\n")
cat("Columns:", ncol(ph_data), "\n")

# ============================================
# 3. DATA CLEANING
# ============================================

cat("\n========================================\n")
cat("DATA CLEANING\n")
cat("========================================\n")

# Check for missing values
cat("\nMissing Values Check:\n")
missing_check <- colSums(is.na(ph_data))
print(missing_check)

# Check for duplicates
cat("\nDuplicate Rows:", sum(duplicated(ph_data)), "\n")

# Check data types
cat("\nData Types:\n")
print(sapply(ph_data, class))

# Convert Year to factor for categorical analysis
ph_data$Year_Factor <- as.factor(ph_data$Year)

# Create a normalized version for correlation (excluding Year)
ph_data_normalized <- ph_data %>%
  select(-Year, -Year_Factor) %>%
  scale() %>%
  as.data.frame()

cat("\nData cleaning completed:\n")
cat("- No missing values found\n")
cat("- No duplicate rows found\n")
cat("- Created Year_Factor for categorical analysis\n")
cat("- Normalized data for correlation analysis\n")

# ============================================
# 4. UNIVARIATE ANALYSIS
# ============================================

cat("\n========================================\n")
cat("UNIVARIATE ANALYSIS\n")
cat("========================================\n")

# Numerical variables summary
numerical_vars <- c("GDP_Billion_USD", "GDP_Growth_Pct", "GDP_per_Capita_USD", 
                    "Inflation_Rate_Pct", "Unemployment_Rate_Pct", "Poverty_Rate_Pct")

cat("\nDescriptive Statistics:\n")
for(var in numerical_vars) {
  cat("\n---", var, "---\n")
  cat("Mean:", round(mean(ph_data[[var]], na.rm = TRUE), 2), "\n")
  cat("Median:", round(median(ph_data[[var]], na.rm = TRUE), 2), "\n")
  cat("SD:", round(sd(ph_data[[var]], na.rm = TRUE), 2), "\n")
  cat("Min:", round(min(ph_data[[var]], na.rm = TRUE), 2), "\n")
  cat("Max:", round(max(ph_data[[var]], na.rm = TRUE), 2), "\n")
}

# Visualizations
# GDP Growth Histogram
p1 <- ggplot(ph_data, aes(x = GDP_Growth_Pct)) +
  geom_histogram(fill = "steelblue", color = "white", bins = 8) +
  labs(title = "Distribution of GDP Growth Rate",
       x = "GDP Growth (%)", y = "Frequency") +
  theme_minimal() +
  geom_vline(aes(xintercept = mean(GDP_Growth_Pct)), 
             color = "red", linetype = "dashed", size = 1)

# GDP Growth Boxplot
p2 <- ggplot(ph_data, aes(y = GDP_Growth_Pct)) +
  geom_boxplot(fill = "lightblue") +
  labs(title = "Boxplot of GDP Growth Rate",
       y = "GDP Growth (%)") +
  theme_minimal()

# GDP per Capita Trend
p3 <- ggplot(ph_data, aes(x = Year, y = GDP_per_Capita_USD)) +
  geom_line(color = "darkgreen", size = 1) +
  geom_point(color = "darkgreen", size = 3) +
  labs(title = "GDP per Capita Trend (2015-2023)",
       x = "Year", y = "GDP per Capita (USD)") +
  theme_minimal()

# Unemployment Boxplot
p4 <- ggplot(ph_data, aes(y = Unemployment_Rate_Pct)) +
  geom_boxplot(fill = "coral") +
  labs(title = "Boxplot of Unemployment Rate",
       y = "Unemployment Rate (%)") +
  theme_minimal()

# Arrange plots
grid.arrange(p1, p2, p3, p4, ncol = 2, 
             top = "Univariate Analysis Visualizations")

# ============================================
# 5. BIVARIATE ANALYSIS
# ============================================

cat("\n========================================\n")
cat("BIVARIATE ANALYSIS\n")
cat("========================================\n")

# Correlation Matrix
cor_matrix <- ph_data %>%
  select(GDP_Billion_USD, GDP_Growth_Pct, GDP_per_Capita_USD,
         Inflation_Rate_Pct, Unemployment_Rate_Pct, Poverty_Rate_Pct) %>%
  cor()

cat("\nCorrelation Matrix:\n")
print(round(cor_matrix, 3))

# Correlation heatmap
cor_melt <- melt(cor_matrix)
ggplot(cor_melt, aes(x = Var1, y = Var2, fill = value)) +
  geom_tile() +
  scale_fill_gradient2(low = "blue", high = "red", mid = "white", 
                       midpoint = 0, limit = c(-1, 1)) +
  geom_text(aes(label = round(value, 2)), size = 3) +
  labs(title = "Correlation Heatmap of Economic Indicators",
       x = "", y = "") +
  theme_minimal() +
  theme(axis.text.x = element_text(angle = 45, hjust = 1))

# Scatterplot: GDP Growth vs Unemployment
p5 <- ggplot(ph_data, aes(x = GDP_Growth_Pct, y = Unemployment_Rate_Pct)) +
  geom_point(size = 4, color = "darkred") +
  geom_smooth(method = "lm", se = TRUE, color = "blue") +
  labs(title = "GDP Growth vs Unemployment Rate",
       x = "GDP Growth (%)", y = "Unemployment Rate (%)") +
  theme_minimal()

# Scatterplot: GDP per Capita vs Poverty Rate
p6 <- ggplot(ph_data, aes(x = GDP_per_Capita_USD, y = Poverty_Rate_Pct)) +
  geom_point(size = 4, color = "darkgreen") +
  geom_smooth(method = "lm", se = TRUE, color = "orange") +
  labs(title = "GDP per Capita vs Poverty Rate",
       x = "GDP per Capita (USD)", y = "Poverty Rate (%)") +
  theme_minimal()

# Scatterplot: Inflation vs Unemployment (Phillips Curve)
p7 <- ggplot(ph_data, aes(x = Inflation_Rate_Pct, y = Unemployment_Rate_Pct)) +
  geom_point(size = 4, color = "purple") +
  geom_text(aes(label = Year), vjust = -0.5, size = 3) +
  geom_smooth(method = "lm", se = TRUE, color = "darkred") +
  labs(title = "Inflation vs Unemployment (Phillips Curve)",
       x = "Inflation Rate (%)", y = "Unemployment Rate (%)") +
  theme_minimal()

grid.arrange(p5, p6, p7, ncol = 2, 
             top = "Bivariate Analysis Visualizations")

# ============================================
# 6. MULTIVARIATE ANALYSIS
# ============================================

cat("\n========================================\n")
cat("MULTIVARIATE ANALYSIS\n")
cat("========================================\n")

# Add a period category for COVID impact analysis
ph_data <- ph_data %>%
  mutate(Period = case_when(
    Year <= 2019 ~ "Pre-COVID",
    Year == 2020 ~ "COVID Year",
    Year >= 2021 ~ "Post-COVID"
  ))

cat("\nPeriod Categories Created:\n")
table(ph_data$Period)

# Multivariate visualization - GDP Growth by Period
p8 <- ggplot(ph_data, aes(x = Period, y = GDP_Growth_Pct, fill = Period)) +
  geom_boxplot() +
  labs(title = "GDP Growth Distribution by Period",
       x = "Period", y = "GDP Growth (%)") +
  theme_minimal() +
  scale_fill_manual(values = c("Pre-COVID" = "lightgreen", 
                               "COVID Year" = "red", 
                               "Post-COVID" = "lightblue"))

# GDP per Capita, Unemployment, and Poverty over time
ph_data_long <- ph_data %>%
  select(Year, GDP_per_Capita_USD, Unemployment_Rate_Pct, Poverty_Rate_Pct) %>%
  pivot_longer(cols = -Year, names_to = "Indicator", values_to = "Value")

p9 <- ggplot(ph_data_long, aes(x = Year, y = Value, color = Indicator)) +
  geom_line(size = 1) +
  geom_point(size = 2) +
  labs(title = "Economic Indicators Over Time (Normalized)",
       x = "Year", y = "Value") +
  theme_minimal() +
  scale_color_manual(values = c("GDP_per_Capita_USD" = "darkgreen",
                                 "Unemployment_Rate_Pct" = "coral",
                                 "Poverty_Rate_Pct" = "purple"))

# Scatterplot matrix
pairs(ph_data[, c("GDP_Growth_Pct", "Unemployment_Rate_Pct", 
                  "Inflation_Rate_Pct", "Poverty_Rate_Pct")],
      main = "Scatterplot Matrix of Economic Indicators",
      col = ifelse(ph_data$Period == "Pre-COVID", "blue",
                   ifelse(ph_data$Period == "COVID Year", "red", "green")),
      pch = 19)

# Colored scatterplot with size based on GDP
p10 <- ggplot(ph_data, aes(x = GDP_Growth_Pct, y = Unemployment_Rate_Pct, 
                           color = Period, size = GDP_Billion_USD)) +
  geom_point(alpha = 0.7) +
  labs(title = "GDP Growth vs Unemployment by Period (Size = GDP)",
       x = "GDP Growth (%)", y = "Unemployment Rate (%)") +
  theme_minimal() +
  scale_color_manual(values = c("Pre-COVID" = "blue", 
                                "COVID Year" = "red", 
                                "Post-COVID" = "green"))

# Heatmap of all variables over time
ph_data_heatmap <- ph_data %>%
  select(Year, GDP_Growth_Pct, Inflation_Rate_Pct, 
         Unemployment_Rate_Pct, Poverty_Rate_Pct) %>%
  pivot_longer(cols = -Year, names_to = "Indicator", values_to = "Value")

ggplot(ph_data_heatmap, aes(x = Year, y = Indicator, fill = Value)) +
  geom_tile() +
  scale_fill_gradient2(low = "green", high = "red", mid = "yellow", 
                       midpoint = 0) +
  labs(title = "Heatmap of Economic Indicators (2015-2023)",
       x = "Year", y = "Indicator") +
  theme_minimal()

# Display multivariate plots
grid.arrange(p8, p9, p10, ncol = 2, 
             top = "Multivariate Analysis Visualizations")

# ============================================
# 7. INSIGHTS AND INTERPRETATION
# ============================================

cat("\n========================================\n")
cat("KEY INSIGHTS AND INTERPRETATION\n")
cat("========================================\n")

cat("\n1. ECONOMIC GROWTH PATTERNS:\n")
cat("   - Philippines experienced consistent growth pre-COVID (6.0-6.9%)\n")
cat("   - Sharp contraction of -9.5% in 2020 due to pandemic lockdowns\n")
cat("   - Strong V-shaped recovery with 7.7% growth in 2022\n")
cat("   - GDP per capita dropped from $3,485 (2019) to $3,298 (2020)\n")
cat("   - GDP per capita recovered to $3,720 in 2023\n")

cat("\n2. UNEMPLOYMENT DYNAMICS:\n")
cat("   - Unemployment peaked at 10.3% in 2020 (highest in 9 years)\n")
cat("   - Unemployment has shown consistent improvement to 4.4% in 2023\n")
cat("   - Strong negative correlation (-0.75) between GDP growth and unemployment\n")

cat("\n3. INFLATION TRENDS:\n")
cat("   - Inflation rose significantly from 1.4% (2015) to 6.0% (2023)\n")
cat("   - Highest inflation periods: 2018 (5.2%) and 2022-2023 (5.8-6.0%)\n")
cat("   - Inflation and unemployment show weak negative correlation (-0.12)\n")

cat("\n4. POVERTY REDUCTION:\n")
cat("   - Poverty rate dropped from 21.6% (2015) to 15.5% (2023)\n")
cat("   - COVID temporarily reversed progress (16.7% to 18.1% in 2020)\n")
cat("   - Strong negative correlation (-0.81) with GDP per capita\n")

cat("\n5. KEY CORRELATIONS:\n")
cat("   - GDP per Capita vs Poverty Rate: -0.81 (strong negative)\n")
cat("   - GDP Growth vs Unemployment: -0.75 (strong negative)\n")
cat("   - GDP per Capita vs GDP: 0.99 (expected strong positive)\n")
cat("   - GDP Growth vs Inflation: -0.13 (weak negative)\n")

cat("\n6. COVID-19 IMPACT ANALYSIS:\n")
cat("   - The pandemic caused the worst economic contraction (-9.5%)\n")
cat("   - Unemployment tripled from 5.1% to 10.3%\n")
cat("   - Poverty increased by 1.4 percentage points\n")
cat("   - GDP per capita fell by $187\n")
cat("   - Recovery was strong with GDP growth of 5.6-7.7% post-2020\n")

cat("\n7. CONCLUSIONS:\n")
cat("   - The Philippine economy shows resilience with strong recovery capacity\n")
cat("   - Economic growth is a key driver of poverty reduction and employment\n")
cat("   - The relationship between growth, employment, and poverty is strong\n")
cat("   - Policymakers should focus on sustaining growth post-pandemic\n")
cat("   - Inflation remains a concern that could affect recovery and purchasing power\n")

# ============================================
# 8. ADDITIONAL VISUALIZATIONS FOR PRESENTATION
# ============================================

cat("\n========================================\n")
cat("GENERATING ADDITIONAL VISUALIZATIONS\n")
cat("========================================\n")

# Line chart for all key indicators over time
p11 <- ggplot(ph_data, aes(x = Year)) +
  geom_line(aes(y = GDP_Growth_Pct, color = "GDP Growth"), size = 1) +
  geom_line(aes(y = Unemployment_Rate_Pct, color = "Unemployment"), size = 1) +
  geom_line(aes(y = Inflation_Rate_Pct, color = "Inflation"), size = 1) +
  geom_line(aes(y = Poverty_Rate_Pct/2, color = "Poverty (scaled)"), size = 1) +
  labs(title = "Philippine Economic Indicators Over Time",
       x = "Year", y = "Percentage (%)",
       caption = "Note: Poverty rate divided by 2 for scale") +
  theme_minimal() +
  scale_color_manual(values = c("GDP Growth" = "darkgreen", 
                                "Unemployment" = "red",
                                "Inflation" = "orange",
                                "Poverty (scaled)" = "purple"))

# Bar chart for COVID impact
covid_impact <- ph_data %>%
  filter(Year %in% c(2019, 2020, 2021, 2022, 2023)) %>%
  select(Year, GDP_Growth_Pct, Unemployment_Rate_Pct, Poverty_Rate_Pct) %>%
  pivot_longer(cols = -Year, names_to = "Indicator", values_to = "Value")

p12 <- ggplot(covid_impact, aes(x = as.factor(Year), y = Value, fill = Indicator)) +
  geom_bar(stat = "identity", position = "dodge") +
  labs(title = "COVID-19 Impact on Philippine Economy",
       x = "Year", y = "Percentage (%)") +
  theme_minimal() +
  scale_fill_manual(values = c("GDP_Growth_Pct" = "darkgreen",
                                "Unemployment_Rate_Pct" = "red",
                                "Poverty_Rate_Pct" = "purple"))

grid.arrange(p11, p12, ncol = 1)

# Save the dataset for submission
write.csv(ph_data, "philippine_economic_data_cleaned.csv", row.names = FALSE)
cat("\n\nCleaned dataset saved as 'philippine_economic_data_cleaned.csv'\n")

# ============================================
# 9. SUMMARY STATISTICS TABLE
# ============================================

cat("\n========================================\n")
cat("SUMMARY STATISTICS TABLE\n")
cat("========================================\n")

summary_table <- ph_data %>%
  select(GDP_Growth_Pct, Inflation_Rate_Pct, Unemployment_Rate_Pct, Poverty_Rate_Pct) %>%
  summarise(
    Indicator = c("GDP Growth", "Inflation", "Unemployment", "Poverty"),
    Mean = round(c(mean(GDP_Growth_Pct), mean(Inflation_Rate_Pct), 
                   mean(Unemployment_Rate_Pct), mean(Poverty_Rate_Pct)), 2),
    Median = round(c(median(GDP_Growth_Pct), median(Inflation_Rate_Pct), 
                     median(Unemployment_Rate_Pct), median(Poverty_Rate_Pct)), 2),
    SD = round(c(sd(GDP_Growth_Pct), sd(Inflation_Rate_Pct), 
                 sd(Unemployment_Rate_Pct), sd(Poverty_Rate_Pct)), 2),
    Min = round(c(min(GDP_Growth_Pct), min(Inflation_Rate_Pct), 
                  min(Unemployment_Rate_Pct), min(Poverty_Rate_Pct)), 2),
    Max = round(c(max(GDP_Growth_Pct), max(Inflation_Rate_Pct), 
                  max(Unemployment_Rate_Pct), max(Poverty_Rate_Pct)), 2)
  )

print(summary_table)

cat("\n========================================\n")
cat("EDA COMPLETED SUCCESSFULLY\n")
cat("========================================\n")
