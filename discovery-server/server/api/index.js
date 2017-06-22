import { Router } from 'express'

import users from './users'
import art28 from './art28'

var router = Router()

// Add USERS Routes
router.use(art28)

export default router
