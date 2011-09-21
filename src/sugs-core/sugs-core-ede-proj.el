(ede-cpp-root-project "sugs-core"
                :name "sugs-core"
                :file "~/src/sugs/src/sugs-core/common.hpp"
                :include-path '("/"
                                "/common"
																"/ext"
																"/fs"
																"/gfx"
																"/messaging"
																"/physics"
																"/worker"
																"/worker/spidermonkey"
                               )
                :system-include-path '("~/include"
																			 "~/src/sugs/extlibs/linux64Release/include"
																			 "~/src/sugs/extlibs/linux64Release/include/js"
																			 )
                :spp-table '(("isUnix" . "")
                             ("SFML_DYNAMIC" . "")))
