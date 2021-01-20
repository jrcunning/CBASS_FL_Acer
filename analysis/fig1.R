library(ggmap) 
library(ggrepel)
library(parzer)
library(tidyverse)

ggmap::register_google(key = readLines("google_api_key"))

# Download satellite map for Florida
map_fl <- get_map(location = c(lon = -80.8, lat = 25.2), zoom = 8,
                  source = "google", maptype = "satellite")

# Set locations of nurseries
loc <- bind_rows("Nova\nSoutheastern\nUniversity" = c(lon = -80.097033, lat = 26.124533), 
                 "University\nof Miami" = c(lon = -80.109067, lat = 25.676267), 
                 "Coral Restoration\nFoundation" = c(lon = -80.43, lat = 24.99), 
                 "Reef Renewal" = c(lon = -80.45, lat = 24.97), 
                 "Florida FWC" = c(lon = -81.025117, lat = 24.667233), 
                 "Mote Marine Lab" = c(lon = -81.40009, lat = 24.56257), 
                 .id = "nursery")

# Add cruise dates for each nursery visited
loc <- loc %>%
  mutate("Cruise Date" = case_when(nursery == "Nova\nSoutheastern\nUniversity" ~ "Aug. 2020",
                                   TRUE ~ "Oct. 2020"))

# Plot map with nursery labels
set.seed(6)
ggmap(map_fl, extent = "device") +
  geom_point(data = loc, aes(x = lon, y = lat, shape = `Cruise Date`), fill = "white", size = 2) +
  scale_shape_manual(values = c(21, 24)) +
  geom_label_repel(dat = loc, aes(label = nursery),
                   fontface = "italic", size = 2,
                   force = 50, segment.size = 0.3, segment.color = "black",
                   direction = "both", hjust = 0.5, vjust = 0.5, nudge_x = 0.5, nudge_y = -0.25) +
  theme(legend.position = "none")

ggsave("output/fig1.png", width = 75, height = 75, units = "mm")
