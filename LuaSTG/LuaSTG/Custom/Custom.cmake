function(luastg_custom __TARGET__ __DIR__)
    target_include_directories(${__TARGET__} PRIVATE
        ${__DIR__}
    )
    set(_LuaSTG_res
        ${__DIR__}/AppFrameLuaEx.cpp
        ${__DIR__}/Config.h
        ${__DIR__}/ResourcePassword.cpp
        ${__DIR__}/app.ico
        ${__DIR__}/resource.h
        ${__DIR__}/resource.rc
    )
    source_group(TREE ${__DIR__} PREFIX LuaSTG/Custom FILES ${_LuaSTG_res})
    target_sources(${__TARGET__} PRIVATE
        ${_LuaSTG_res}
    )
endfunction()
