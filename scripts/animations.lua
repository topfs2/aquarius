local animations = { }

animations.AnimatedSpriteSheet = function(e, f, x, y, angle, w, h, frames, fps)
    local that = { [ "frame" ] = 0, [ "loop" ] = false, [ "autoremove" ] = true }

    local u0 = 0
    local v0 = 0
    local uf = 1 / frames
    local vf = 1

    e:assign(Position(x, y, angle))
    e:assign(Sprite(f, w, h, u0, v0, u0 + uf, v0 + vf))

    that.update = function(dt)
        that.frame = that.frame + dt * fps

        if (that.frame > frames and that.loop) then
            that.frame = that.frame - frames
        elseif (that.frame > frames) then
            that.frame = frames - 1
            if (that.autoremove) then
                that.update = function(dt) end
                that.destroy()
            end
        end

        u0 = math.floor(that.frame) / frames

        if (e:valid()) then
            e.sprite.u0 = u0
            e.sprite.u1 = u0 + uf
        end
    end

    that.destroy = function()
        if (e:valid()) then
            e:destroy()
        end
    end

    that.valid = function ()
        return e:valid()
    end

    return that
end

animations.AnimatedSprite = function(e, fbase, numbers, ext, x, y, angle, w, h, frames, fps)
    local format = function (i)
        return string.format ("%s%0" .. numbers .. "i.%s", fbase, i, ext)
    end

    local that = { [ "frame" ] = 0, [ "loop" ] = false, [ "autoremove" ] = true }

    local e = EntityManager:create()
    e:assign(Position(x, y, angle))
    e:assign(Sprite(format(0), w, h))

    that.update = function(dt)
        that.frame = that.frame + dt * fps

        if (that.frame > frames and that.loop) then
            that.frame = that.frame - frames
        elseif (that.frame > frames) then
            that.frame = frames - 1
            if (that.autoremove) then
                that.update = function(dt) end
                that.destroy()
            end
        end

        if (e:valid()) then
            e.sprite.filename = format(math.floor(that.frame))
        end
    end

    that.destroy = function()
        if (e:valid()) then
            e:destroy()
        end
    end

    that.valid = function ()
        return e:valid()
    end

    return that
end

return animations
