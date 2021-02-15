library(tidyverse)
library(lubridate)
#library(hms)
library(drc)

# Panel 1: CBASS set points
myTime <- now() %>% floor_date("day")

myData <- tibble(max_temp = c(30, 32, 33, 34, 35, 36, 37, 38),
               time = list(seq(myTime, myTime + hm("09:00"), by = "1 min"))) %>%
  mutate(set_point = map(max_temp, ~ c(
    rep(30, 61),
    seq(30, .x, len = 180),
    rep(.x, 180),
    seq(.x, 30, len = 60),
    rep(30, 60)
  )))

df <- myData %>% unnest(time, set_point)

p1 <- ggplot(df, aes(x = time, y = set_point, group = factor(max_temp))) +
  geom_rect(aes(xmin = myTime + hm("07:00"), xmax = myTime + hm("09:00"), 
                ymin = -Inf, ymax = Inf), fill = "lightgray", alpha = 0.9999) +
  geom_line(aes(color = set_point), lwd = 1) +
  scale_color_gradient2(midpoint=34, low="#4575b4", mid="#fee090", high="#d73027") +
  scale_x_datetime(breaks = "1 hour", expand = c(0,0),
                   labels = function(x) paste0(difftime(x, myTime + hm("07:00"), units = "hours"), ":00")) +
  labs(x = "Time from local sunset", y = "Temperature set point (°C)") +
  annotate("text", x = myTime + hm("08:30"), y = 30.1, angle = 90, adj = 0,
           label = expression(paste("} \u2013 " , italic(F[V]/F[M]), " measured"))) +
  theme_few() +
  theme(legend.position = "none")


# Panel 2: dose-response curve
td <- tribble(
  ~max_temp, ~fvfm1, ~fvfm2,
  30, 0.679, 0.694, 
  32, 0.678, 0.685, 
  33, 0.672, 0.658,
  34, 0.589, 0.611,
  35, 0.465, 0.512,
  36, 0.351, 0.300, 
  37, 0.155, 0.190,
  38, 0.076, 0.111)

td <- pivot_longer(td, starts_with("fvfm"), values_to = "fvfm")

mod <- drm(fvfm ~ max_temp, data = td, fct = LL.3())

ndf <- data.frame(max_temp = seq(30, 38, 0.1)) %>%
  mutate(fvfm = predict(mod, newdata = data.frame(max_temp = seq(30, 38, 0.1))))
ed50 <- coef(mod)[3]

p2 <- ggplot(td, aes(x = max_temp, y = fvfm)) +
  geom_point(aes(color = max_temp), size = 2) +
  scale_color_gradient2(midpoint=34, low="#4575b4", mid="#fee090", high="#d73027") +
  geom_line(data = ndf) +
  geom_vline(aes(xintercept = ed50), lty = 2) +
  annotate("text", x = ed50 - 0.2, y = 0.1, adj = 1,
           label = paste0("ED50 = ", round(ed50, 2), "°C")) +
  scale_y_continuous(limits = c(0, 0.75)) +
  theme_few() +
  theme(legend.position = "none") +
  labs(y = expression(italic(F[V]/F[M])), x = "Maximum temperature (°C)")


# Composite figure
fig2 <- cowplot::plot_grid(p1, p2,
                           labels = "AUTO")

ggsave("output/fig2.png", width = 172, height = 75, units = "mm")


