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
																"../../extlibs/linux64Release/include"
															  "../../extlibs/linux64Release/include/js"
															  "../../extlibs/linux64Release/include/boost"
                               )
                :system-include-path '("~/include"
																			 "~/src/sugs/extlibs/linux64Release/include"
																			 "~/src/sugs/extlibs/linux64Release/include/js"
																			 ))

(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include" 'c-mode)
(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include" 'c++-mode)
(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include/js" 'c-mode)
(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include/js" 'c++-mode)
(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include/boost" 'c-mode)
(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include/boost" 'c++-mode)
(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include/chipmunk" 'c-mode)
(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include/chipmunk" 'c++-mode)
(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include/SFML" 'c-mode)
(semantic-add-system-include "~/src/sugs/extlibs/linux64Release/include/SFML" 'c++-mode)
