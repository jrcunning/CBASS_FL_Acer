library(ggrepel)
library(parzer)
library(tidyverse)
library(lubridate)
library(drc)

library("rnaturalearth")
library("rnaturalearthdata")
library("sf")



# Download satellite map for Florida
world <- ne_countries(scale = "large", returnclass = "sf")

# Set locations of nurseries
loc <- bind_rows("NSU\n(n = 37)" = c(lon = -80.097033, lat = 26.124533), 
                 "UM\n(n = 41)" = c(lon = -80.109067, lat = 25.676267), 
                 "CRF\n(n = 44)" = c(lon = -80.43, lat = 24.99), 
                 "RR\n(n = 42)" = c(lon = -80.45, lat = 24.97), 
                 "FWC\n(n = 25)" = c(lon = -81.025117, lat = 24.667233), 
                 "MML\n(n = 40)" = c(lon = -81.40009, lat = 24.56257), 
                 .id = "nursery") %>%
  mutate(nursery = fct_reorder(nursery, lon)) %>%
  arrange(nursery)

# Plot map with nursery labels
set.seed(10)
fig1a <- #ggmap(map_fl, extent = "panel") +
  ggplot(data = world) +
  geom_sf(lwd = 0, fill = "gray70") +
  coord_sf(xlim = c(-82.6, -79.1), ylim = c(23.7, 26.9), expand = FALSE) +
  geom_point(data = loc, aes(x = lon, y = lat), pch = 21, fill = "white", size = 1.5, stroke = 0.3) +
  annotate(geom = "point", x = -80.1918, y = 25.7617, size = 0.5) +
  annotate(geom = "text", x = -80.25, y = 25.85, label = "Miami", adj = 1, size = 2, fontface = "italic") +
  annotate(geom = "point", x = -81.78, y = 24.5551, size = 0.5) +
  annotate(geom = "text", x = -82.03, y = 24.67, label = "Key West", adj = 0.5, size = 2, fontface = "italic") +
  geom_label_repel(dat = loc, aes(x = lon, y = lat, label = nursery, fill = nursery), 
                   label.padding = 0.125,
                   size = 2,
                   force = 50, segment.size = 0.3, segment.color = "black", min.segment.length = 0,
                   direction = "both", hjust = 0.5, vjust = 0.5, 
                   nudge_x = c(-0.6, -0.15, 0.05, 0.5, 0.5, 0.5), 
                   nudge_y = c(-0.5, -0.5, -0.5, -0.05, 0, 0.2)) +
  scale_fill_manual(values = alpha(c("#F8766D", "#B79F00", "#00BA38", 
                                     "#00BFC4", "#619CFF", "#F564E3"), 0.7)) +
  scale_y_continuous(breaks = c(24, 25, 26), labels = c(24, 25, 26)) +
  scale_x_continuous(breaks = c(-82, -81, -80), labels = c(-82, -81, -80)) +
  theme(legend.position = "none") +
  theme(text = element_text(size = 10),
        panel.background = element_blank(),
        panel.border = element_rect(colour = "black", fill=NA)) +
  labs(y = "Latitude", x = "Longitude")
fig1a



# Panel B: CBASS set points
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
  geom_line(aes(color = set_point), lwd = 0.75) +
  scale_color_gradient2(midpoint=34, low="#4575b4", mid="#fee090", high="#d73027") +
  scale_x_datetime(breaks = "1 hour", expand = c(0,0),
                   labels = function(x) difftime(x, myTime + hm("07:00"), units = "hours")) +
  labs(x = "Hours from local sunset", y = "Temperature (°C)") +
  annotate("text", x = myTime + hm("08:30"), y = 30.1, angle = 90, adj = 0, size = 2,
           label = expression(paste("} \u2013 " , italic(F[V]/F[M]), " measured"))) +
  theme_few() +
  theme(text = element_text(size = 10)) +
  theme(legend.position = "none")


# Panel C: dose-response curve
td <- tribble(
  ~max_temp, ~fvfm1, ~fvfm2,
  30, 0.674, 0.699, 
  32, 0.671, 0.692, 
  33, 0.678, 0.652,
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
  geom_point(aes(color = max_temp), size = 1.5) +
  scale_color_gradient2(midpoint=34, low="#4575b4", mid="#fee090", high="#d73027") +
  geom_line(data = ndf) +
  geom_vline(aes(xintercept = ed50), lty = 2) +
  annotate("text", x = ed50 - 0.2, y = 0.1, adj = 1,
           label = paste0("ED50 = ", round(ed50, 2), "°C"), size = 2) +
  scale_y_continuous(limits = c(0, 0.75)) +
  theme_few() +
  theme(text = element_text(size = 10)) +
  theme(legend.position = "none") +
  labs(y = expression(italic(F[V]/F[M])), x = "Maximum temperature (°C)") 


# Composite figure
set.seed(90)
fig1 <- cowplot::plot_grid(fig1a, p1, p2, ncol = 3,
                           labels = "AUTO")

ggsave("output/fig1.png", width = 180, height = 60, units = "mm")


