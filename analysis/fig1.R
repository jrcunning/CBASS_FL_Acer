library(ggmap) 
library(ggrepel)
library(parzer)
library(tidyverse)
library(lubridate)
library(drc)

ggmap::register_google(key = readLines("google_api_key"))

# Download satellite map for Florida
map_fl <- get_map(location = c(lon = -80.8, lat = 25.2), zoom = 8,
                  source = "google", maptype = "satellite")

# Set locations of nurseries
# loc <- bind_rows("Nova\nSoutheastern\nUniversity\n(n = 37)" = c(lon = -80.097033, lat = 26.124533), 
#                  "University\nof Miami\n(n = 41)" = c(lon = -80.109067, lat = 25.676267), 
#                  "Coral\nRestoration\nFoundation\n(n = 44)" = c(lon = -80.43, lat = 24.99), 
#                  "Reef Renewal\n(n = 42)" = c(lon = -80.45, lat = 24.97), 
#                  "Florida FWC\n(n = 25)" = c(lon = -81.025117, lat = 24.667233), 
#                  "Mote\nMarine Lab\n(n = 40)" = c(lon = -81.40009, lat = 24.56257), 
#                  .id = "nursery")

loc <- bind_rows("NSU\n(n = 37)" = c(lon = -80.097033, lat = 26.124533), 
                 "UM\n(n = 41)" = c(lon = -80.109067, lat = 25.676267), 
                 "CRF\n(n = 44)" = c(lon = -80.43, lat = 24.99), 
                 "RR\n(n = 42)" = c(lon = -80.45, lat = 24.97), 
                 "FWC\n(n = 25)" = c(lon = -81.025117, lat = 24.667233), 
                 "MML\n(n = 40)" = c(lon = -81.40009, lat = 24.56257), 
                 .id = "nursery") %>%
  mutate(nursery = fct_reorder(nursery, lon)) %>%
  arrange(nursery)

# Add cruise dates for each nursery visited
# loc <- loc %>%
#   mutate("Cruise Date" = case_when(nursery == "Nova\nSoutheastern\nUniversity" ~ "Aug. 2020",
#                                    TRUE ~ "Oct. 2020"))

# Plot map with nursery labels
set.seed(10)
fig1a <- ggmap(map_fl, extent = "panel") +
  geom_point(data = loc, aes(x = lon, y = lat), pch = 21, fill = "white", size = 2) +
  scale_shape_manual(values = c(21, 24)) +
  geom_label_repel(dat = loc, aes(label = nursery), label.padding = 0.125,
                   fontface = "italic", size = 1.5,
                   force = 50, segment.size = 0.3, segment.color = "black", min.segment.length = 0,
                   direction = "both", hjust = 0.5, vjust = 0.5, 
                   nudge_x = c(-0.6, -0.15, 0.05, 0.5, 0.5, 0.5), 
                   nudge_y = c(-0.5, -0.5, -0.5, -0.05, 0, 0.2)) +
  theme(legend.position = "none") +
  theme(text = element_text(size = 10)) +
  labs(x = "Latitude", y = "Longitude")
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
  geom_line(aes(color = set_point), lwd = 1) +
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
fig1

ggsave("output/fig1.png", width = 180, height = 60, units = "mm")


