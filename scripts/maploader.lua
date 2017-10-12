local loader = function (map)
    local spawns = { ["player"] = { } }

    local tileset = map.tilesets[1]

    local coord = function (i, w)
        local y = math.floor(i / w)
        local x = i % w

        return x, y
    end

    local lookup = function (i, tileset)
        local iw = tileset.imagewidth
        local ih = tileset.imageheight
        local tw = iw / tileset.tilewidth
        local th = ih / tileset.tileheight
        local f  = "textures/" .. tileset.image


        local tx, ty = coord(i, tw)

        local u0 = tx / tw
        local v0 = ty / th
        local u1 = (tx + 1) / tw
        local v1 = (ty + 1) / th

        return f, u0, v0, u1, v1
    end

    local temp = {
      type = "objectgroup",
      name = "GroundLayer",
      visible = true,
      opacity = 1,
      properties = {},
      objects = {
        {
          name = "",
          type = "",
          shape = "rectangle",
          x = 32,
          y = 224,
          width = 192,
          height = 32,
          visible = true,
          properties = {}
        }
      }
    }

    local create_collision = function (o)
        local sx = tileset.tilewidth
        local sy = tileset.tileheight

        if (o.shape == "rectangle") then
            local w =  o.width
            local h =  o.height

            local x =  o.x + w * 0.5
            local y = -o.y - h * 0.5

            w = w / sx
            h = h / sy
            x = x / sx
            y = y / sy

            local e = EntityManager:create()
            e:assign(Position(x, y, 0))
            e:assign(physics.Body(Rectangle(w, h), 0, 0.3))

        elseif (o.shape == "polygon") then
            local x =  o.x / sx
            local y = -o.y / sy

            local polygon = Polygon()

            for j,v in pairs(o.polygon) do
                polygon:push_back(vec2(v.x / sx, -v.y / sy))
            end

            local e = EntityManager:create()
            e:assign(Position(x, y, 0))
            e:assign(physics.Body(polygon, 0, 0.3))
        elseif (o.shape == "polyline") then
            local x =  o.x / sx
            local y = -o.y / sy

            local line = Line()

            for j,v in pairs(o.polyline) do
                line:push_back(vec2(v.x / sx, -v.y / sy))
            end

            local e = EntityManager:create()
            e:assign(Position(x, y, 0))
            e:assign(physics.Body(line, 0, 0.3))
        else
            print ("Unknown shape", o.shape)
        end
    end

    local create_physicslayer = function (layer)
        local sx = tileset.tilewidth
        local sy = tileset.tileheight

        print("Creating physics layer", layer.name)

        for i,o in pairs(layer.objects) do
            print("Creating", o.shape, o.type)

            if (o.type == "spawn.player") then
                table.insert(spawns.player, vec2(o.x / sx, -o.y / sx))
            else
                create_collision(o)
            end
        end
    end

    local lz = -1

    local create_spritelayer = function (layer)
        print("Creating sprite layer", layer.name)
        local z = layer.properties.z or lz
        EventManager:emit(CreateLayerEvent(layer.name, 0.0, 0.0, tonumber(z)))
        lz = lz + 0.01

        local w = layer.width
        local h = layer.height

        local ox = 0---w / 2
        local oy = 0-- h / 2

        for i,t in pairs(layer.data) do
            if (t > 0) then
                local x, y = coord(i - 1, w)
                local f, u0, v0, u1, v1 = lookup(t - 1, tileset)

                local e = EntityManager:create()
                e:assign(Sprite(f, 1, 1, u0, v0, u1, v1))
                e:assign(Position(x + 0.5, -y - 0.5, 0))
                e:assign(Layer(layer.name))
            end
        end
    end

    local create_layer = function (layer)
        if (layer.visible) then
            if (layer.type == "tilelayer") then
                create_spritelayer(layer)
            elseif (layer.type == "objectgroup") then
                create_physicslayer(layer)
            else
                print("unknown layer type", layer.type)
            end
        end
    end

    local layers = map.layers
    for k,v in pairs(layers) do
        create_layer(v)
    end

    return spawns
end

return loader;
