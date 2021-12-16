# Function to help manage building shaders.
FUNCTION( GLSL_COMPILE )
  
  CMAKE_POLICY( SET CMP0057 NEW )

  # Test configurations.
  SET( VARIABLES 
        TARGETS
        INCLUDE_DIR
        NAME
     )
  
  SET( SHOULD_REBUILD FALSE )

  # For each argument provided.
  FOREACH( ARG ${ARGV} )
    # If argument is one of the variables, set it.
    IF( "${ARG}" IN_LIST VARIABLES )
      SET( STATE ${ARG} )
    ELSE()
      # If our state is a variable, set that variables value
      IF( "${${STATE}}" )
        SET( ${STATE} ${ARG} )
      ELSE()
        LIST( APPEND ${STATE} ${ARG} )
      ENDIF()

      # If our state is a setter, set the value in the parent scope as well
      IF( "${STATE}" IN_LIST CONFIGS )
        SET( ${STATE} ${${STATE}} PARENT_SCOPE )
      ENDIF()
    ENDIF()
  ENDFOREACH()

    IF( TARGETS )
      IF( COMPILE_GLSL )
        # Check if we can build .imp files with impReflection.
        FIND_PROGRAM( HAS_CATAREFLECTION "cataReflection" )

        # Build .imp files using impReflection.
        IF( HAS_CATAREFLECTION )
          ADD_CUSTOM_COMMAND(
            OUTPUT ${SHADER_DIR}/${NAME}.h
            COMMAND cataReflection -v -h ${TARGETS} -o ${SHADER_DIR}/${NAME}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${TARGETS}
          )
    
          ADD_CUSTOM_TARGET(
            ${NAME}_compiled ALL
            DEPENDS ${SHADER_DIR}/${NAME}.h
          )
        ELSE()
          MESSAGE( "Tried to build .cata file using ${TARGETS}, but cataReflection was not found." )
        ENDIF()
      ENDIF()
    ENDIF()
ENDFUNCTION()