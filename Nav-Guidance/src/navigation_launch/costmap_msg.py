from map_msg import MapData


class CostmapData(MapData):

    def __init__(self, transform, costmap_bytes, map_bytes):
        MapData.__init__(self, transform, map_bytes)
        self.costmap_bytes = costmap_bytes