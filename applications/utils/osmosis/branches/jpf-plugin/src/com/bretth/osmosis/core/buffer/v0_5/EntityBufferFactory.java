// License: GPL. Copyright 2007-2008 by Brett Henderson and other contributors.
package com.bretth.osmosis.core.buffer.v0_5;

import com.bretth.osmosis.core.pipeline.common.TaskConfiguration;
import com.bretth.osmosis.core.pipeline.common.TaskManager;
import com.bretth.osmosis.core.pipeline.common.TaskManagerFactory;
import com.bretth.osmosis.core.pipeline.v0_5.SinkRunnableSourceManager;


/**
 * The task manager factory for an entity buffer.
 * 
 * @author Brett Henderson
 */
public class EntityBufferFactory extends TaskManagerFactory {
	private static final String ARG_BUFFER_CAPACITY = "bufferCapacity";
	private static final int DEFAULT_BUFFER_CAPACITY = 100;
	
	
	/**
	 * {@inheritDoc}
	 */
	@Override
	protected TaskManager createTaskManagerImpl(TaskConfiguration taskConfig) {
		int bufferCapacity;
		
		// Get the task arguments.
		bufferCapacity = getIntegerArgument(
			taskConfig,
			ARG_BUFFER_CAPACITY,
			getDefaultIntegerArgument(taskConfig, DEFAULT_BUFFER_CAPACITY)
		);
		
		return new SinkRunnableSourceManager(
			taskConfig.getId(),
			new EntityBuffer(bufferCapacity),
			taskConfig.getPipeArgs()
		);
	}
}
